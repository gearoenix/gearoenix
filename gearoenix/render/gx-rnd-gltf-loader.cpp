#include "gx-rnd-gltf-loader.hpp"
#include "../physics/gx-phs-transformation.hpp"
#include "../platform/gx-plt-log.hpp"
#include "../platform/stream/gx-plt-stm-path.hpp"
#include "../platform/stream/gx-plt-stm-stream.hpp"
#include "camera/gx-rnd-cmr-builder.hpp"
#include "camera/gx-rnd-cmr-camera.hpp"
#include "camera/gx-rnd-cmr-manager.hpp"
#include "engine/gx-rnd-eng-engine.hpp"
#include "mesh/gx-rnd-msh-manager.hpp"
#include "mesh/gx-rnd-msh-mesh.hpp"
#include "model/gx-rnd-mdl-builder.hpp"
#include "model/gx-rnd-mdl-manager.hpp"
#include "scene/gx-rnd-scn-builder.hpp"
#include "scene/gx-rnd-scn-manager.hpp"
#include "texture/gx-rnd-txt-manager.hpp"
#include "texture/gx-rnd-txt-texture-2d.hpp"
#include "../physics/animation/gx-phs-anm-bone.hpp"
#include "../physics/animation/gx-phs-anm-manager.hpp"
#include "../physics/gx-phs-engine.hpp"
#include "light/gx-rnd-lt-manager.hpp"
#include "light/gx-rnd-lt-builder.hpp"
#include "light/gx-rnd-lt-light.hpp"
#include <tuple>
#include <functional>

#define TINYGLTF_NOEXCEPTION
#define TINYGLTF_IMPLEMENTATION
#include <tiny_gltf.h>

std::vector<std::shared_ptr<gearoenix::render::scene::Builder>> gearoenix::render::load_gltf(
    engine::Engine& e,
    const platform::stream::Path& file,
    const core::sync::EndCallerIgnored& end_callback) noexcept
{
    tinygltf::TinyGLTF context;
    tinygltf::Model data;

    std::vector<std::uint8_t> bones_memory;
    std::vector<physics::animation::Bone::Index> bones_memory_indices;
    boost::container::flat_map<int, physics::animation::Bone::Index> node_bone_memory_index;
    std::vector<physics::animation::Bone::Index> bone_children_indices;

    std::string err, warn;
    const auto stream = platform::stream::Stream::open(file, e.get_platform_application());
    GX_ASSERT(nullptr != stream);
    const auto file_data = stream->get_file_content();
    if (!context.LoadBinaryFromMemory(&data, &err, &warn, file_data.data(), static_cast<unsigned int>(file_data.size())))
        GX_LOG_F("Error in GLTF loader: " << err);
    if (!warn.empty())
        GX_LOG_E("Warning in GLTF loader: " << warn);
    if (!err.empty())
        GX_LOG_F("Error in GLTF loader: " << err);

    std::vector<std::shared_ptr<scene::Builder>> result;
    result.reserve(data.scenes.size());

    std::vector<std::shared_ptr<texture::Texture2D>> gx_txt2ds(data.textures.size());
    const auto create_texture2d = [&](const int index, const core::sync::EndCallerIgnored& txt_end_callback, const std::shared_ptr<texture::Texture2D>& default_value) noexcept -> const std::shared_ptr<texture::Texture2D>& {
        if (index < 0)
            return default_value;
        auto& gx_txt = gx_txt2ds[index];
        if (nullptr != gx_txt)
            return gx_txt;
        const tinygltf::Texture& txt = data.textures[index];
        GX_LOG_D("Loading texture: " << txt.name);
        const tinygltf::Image& img = data.images[txt.source];
        GX_LOG_D("Loading image: " << img.name);
        GX_ASSERT(!img.as_is); // Only image through bufferView is supported.
        GX_ASSERT(img.bufferView >= 0); // Only image through bufferView is supported.
        GX_ASSERT(img.mimeType.ends_with("jpeg") || img.mimeType.ends_with("png")); // Only these formats are supported.
        const tinygltf::BufferView& img_bv = data.bufferViews[img.bufferView];
        const tinygltf::Buffer& img_b = data.buffers[img_bv.buffer];
        const void* const img_ptr = &img_b.data[img_bv.byteOffset];
        const auto img_sz = static_cast<std::size_t>(img_bv.byteLength);
        const tinygltf::Sampler& smp = data.samplers[txt.sampler];
        GX_LOG_D("Loading sampler: " << smp.name);
        bool needs_mipmap = false;
        const auto convert_filter = [&](const int f) {
            switch (f) {
            case TINYGLTF_TEXTURE_FILTER_LINEAR:
                return texture::Filter::Linear;
            case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR:
                needs_mipmap = true;
                return texture::Filter::LinearMipmapLinear;
            case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST:
                needs_mipmap = true;
                return texture::Filter::LinearMipmapNearest;
            case TINYGLTF_TEXTURE_FILTER_NEAREST:
                return texture::Filter::Nearest;
            case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR:
                needs_mipmap = true;
                return texture::Filter::NearestMipmapLinear;
            case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST:
                needs_mipmap = true;
                return texture::Filter::NearestMipmapNearest;
            default:
                GX_UNEXPECTED;
            }
        };
        const auto convert_wrap = [&](const int w) {
            switch (w) {
            case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE:
                return texture::Wrap::ClampToEdge;
            case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT:
                return texture::Wrap::Mirror;
            case TINYGLTF_TEXTURE_WRAP_REPEAT:
                return texture::Wrap::Repeat;
            default:
                GX_UNEXPECTED;
            }
        };
        const texture::TextureInfo txt_info {
            .format = texture::TextureFormat::Unknown, // [&] {
                                                       // if (2 == img.component && 8 == img.bits && TINYGLTF_COMPONENT_TYPE_BYTE == img.pixel_type) return texture::TextureFormat::RgbaUint8;
                                                       // if (2 == img.component && 8 == img.bits && TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE == img.pixel_type) return texture::TextureFormat::RgbaUint8;
                                                       // if (3 == img.component && 8 == img.bits && TINYGLTF_COMPONENT_TYPE_BYTE == img.pixel_type) return texture::TextureFormat::RgbaUint8;
                                                       // if (3 == img.component && 8 == img.bits && TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE == img.pixel_type) return texture::TextureFormat::RgbaUint8;
                                                       // if (4 == img.component && 8 == img.bits && TINYGLTF_COMPONENT_TYPE_BYTE == img.pixel_type) return texture::TextureFormat::RgbaUint8;
                                                       // if (4 == img.component && 8 == img.bits && TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE == img.pixel_type) return texture::TextureFormat::RgbaUint8;
                                                       // if (4 == img.component && 32 == img.bits && TINYGLTF_COMPONENT_TYPE_FLOAT == img.pixel_type) return texture::TextureFormat::RgbaFloat32;
                                                       // GX_UNIMPLEMENTED;
            //}(),
            .sampler_info = texture::SamplerInfo {
                .min_filter = convert_filter(smp.minFilter),
                .mag_filter = convert_filter(smp.magFilter),
                .wrap_s = convert_wrap(smp.wrapS),
                .wrap_t = convert_wrap(smp.wrapT),
            },
            .width = 0, // static_cast<std::size_t>(img.width),
            .height = 0, // static_cast<std::size_t>(img.height),
            .depth = 0,
            .type = texture::Type::Unknown, // texture::Type::Texture2D,
            .has_mipmap = needs_mipmap,
        };
        gx_txt = e.get_texture_manager()->create_2d_from_formatted(txt.name, img_ptr, img_sz, txt_info, txt_end_callback);
        return gx_txt;
    };

    std::vector<std::shared_ptr<mesh::Mesh>> gx_meshes(data.meshes.size());
    const auto create_mesh = [&](const int index, material::Pbr& gx_mat, const core::sync::EndCallerIgnored& mesh_end_callback) noexcept -> const std::shared_ptr<mesh::Mesh>& {
        auto& gx_mesh = gx_meshes[index];
        if (nullptr != gx_mesh)
            return gx_mesh;
        const tinygltf::Mesh& msh = data.meshes[index];
        GX_LOG_D("Loading mesh: " << msh.name);
        GX_ASSERT(msh.primitives.size() == 1); // Only one primitive for each mesh
        const tinygltf::Primitive& primitive = msh.primitives[0];
        GX_ASSERT(TINYGLTF_MODE_TRIANGLES == primitive.mode);
        const auto& acs = data.accessors;
        const auto& bvs = data.bufferViews;
        const auto& bfs = data.buffers;
        auto pos_ai = static_cast<std::size_t>(-1);
        auto nrm_ai = static_cast<std::size_t>(-1);
        auto tng_ai = static_cast<std::size_t>(-1);
        auto txc_ai = static_cast<std::size_t>(-1);
        auto bwt_ai = static_cast<std::size_t>(-1);
        auto bin_ai = static_cast<std::size_t>(-1);
        const auto& attrs = primitive.attributes;
        GX_ASSERT(attrs.size() == 4 || attrs.size() == 6);
        const bool is_animated = attrs.size() == 6;
        for (const auto& att : attrs) {
            if ("POSITION" == att.first) {
                pos_ai = static_cast<std::size_t>(att.second);
            } else if ("NORMAL" == att.first) {
                nrm_ai = static_cast<std::size_t>(att.second);
            } else if ("TANGENT" == att.first) {
                tng_ai = static_cast<std::size_t>(att.second);
            } else if ("TEXCOORD_0" == att.first) {
                txc_ai = static_cast<std::size_t>(att.second);
            } else if (is_animated && "WEIGHTS_0" == att.first) {
                bwt_ai = static_cast<std::size_t>(att.second);
            } else if (is_animated && "JOINTS_0" == att.first) {
                bin_ai = static_cast<std::size_t>(att.second);
            } else
                GX_UNEXPECTED;
        }
        GX_ASSERT(pos_ai != static_cast<std::size_t>(-1));
        GX_ASSERT(nrm_ai != static_cast<std::size_t>(-1));
        GX_ASSERT(tng_ai != static_cast<std::size_t>(-1));
        GX_ASSERT(txc_ai != static_cast<std::size_t>(-1));
        GX_ASSERT(!is_animated || bwt_ai != static_cast<std::size_t>(-1));
        GX_ASSERT(!is_animated || bin_ai != static_cast<std::size_t>(-1));

        GX_ASSERT(-1 != primitive.indices);
        GX_ASSERT(-1 != primitive.material);

        const auto& pos_a = acs[pos_ai];
        const auto& nrm_a = acs[nrm_ai];
        const auto& tng_a = acs[tng_ai];
        const auto& txc_a = acs[txc_ai];
        const auto& bwt_a = is_animated ? acs[bwt_ai] : acs[txc_ai];
        const auto& bin_a = is_animated ? acs[bin_ai] : acs[txc_ai];
        const auto& ids_a = acs[primitive.indices];

        GX_CHECK_EQUAL(pos_a.count, nrm_a.count);
        GX_CHECK_EQUAL(pos_a.count, tng_a.count);
        GX_CHECK_EQUAL(pos_a.count, txc_a.count);
        GX_ASSERT(!is_animated || pos_a.count == bwt_a.count);
        GX_ASSERT(!is_animated || pos_a.count == bin_a.count);

        GX_CHECK_EQUAL(pos_a.type, TINYGLTF_TYPE_VEC3);
        GX_CHECK_EQUAL(nrm_a.type, TINYGLTF_TYPE_VEC3);
        GX_CHECK_EQUAL(tng_a.type, TINYGLTF_TYPE_VEC4);
        GX_CHECK_EQUAL(txc_a.type, TINYGLTF_TYPE_VEC2);
        GX_ASSERT(!is_animated || bwt_a.type == TINYGLTF_TYPE_VEC4);
        GX_ASSERT(!is_animated || bin_a.type == TINYGLTF_TYPE_VEC4);
        GX_CHECK_EQUAL(ids_a.type, TINYGLTF_TYPE_SCALAR);

        GX_CHECK_EQUAL(pos_a.componentType, TINYGLTF_COMPONENT_TYPE_FLOAT);
        GX_CHECK_EQUAL(nrm_a.componentType, TINYGLTF_COMPONENT_TYPE_FLOAT);
        GX_CHECK_EQUAL(tng_a.componentType, TINYGLTF_COMPONENT_TYPE_FLOAT);
        GX_CHECK_EQUAL(txc_a.componentType, TINYGLTF_COMPONENT_TYPE_FLOAT);
        GX_ASSERT(!is_animated || bwt_a.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

        const auto& pos_max = pos_a.maxValues;
        const auto& pos_min = pos_a.minValues;
        GX_CHECK_EQUAL(pos_max.size(), 3);
        GX_CHECK_EQUAL(pos_min.size(), 3);

        math::Aabb3<double> box(
            math::Vec3<double>(pos_max[0], pos_max[1], pos_max[2]),
            math::Vec3<double>(pos_min[0], pos_min[1], pos_min[2]));

        std::vector<PbrVertex> vertices(pos_a.count);
        std::vector<PbrVertexAnimated> animated_vertices(is_animated? pos_a.count: 0);
        std::vector<std::uint32_t> indices(ids_a.count);

        const auto& pos_bv = bvs[pos_a.bufferView];
        const auto& nrm_bv = bvs[nrm_a.bufferView];
        const auto& tng_bv = bvs[tng_a.bufferView];
        const auto& txc_bv = bvs[txc_a.bufferView];
        const auto& bwt_bv = is_animated ? bvs[bwt_a.bufferView] : bvs[txc_a.bufferView];
        const auto& bin_bv = is_animated ? bvs[bin_a.bufferView] : bvs[txc_a.bufferView];
        const auto& ids_bv = bvs[ids_a.bufferView];

        {
            const auto* const pos_b = &bfs[pos_bv.buffer].data[pos_bv.byteOffset];
            const std::size_t pos_bi_inc = pos_bv.byteStride > 0 ? pos_bv.byteStride : sizeof(math::Vec3<float>);
            std::size_t bi = 0;
            for (auto& vertex : vertices) {
                vertex.position = *reinterpret_cast<const math::Vec3<float>*>(&pos_b[bi]);
                bi += pos_bi_inc;
            }
        }

        {
            const auto* const nrm_b = &bfs[nrm_bv.buffer].data[nrm_bv.byteOffset];
            const std::size_t nrm_bi_inc = nrm_bv.byteStride > 0 ? nrm_bv.byteStride : sizeof(math::Vec3<float>);
            std::size_t bi = 0;
            for (auto& vertex : vertices) {
                vertex.normal = *reinterpret_cast<const math::Vec3<float>*>(&nrm_b[bi]);
                bi += nrm_bi_inc;
            }
        }

        {
            const auto* const tng_b = &bfs[tng_bv.buffer].data[tng_bv.byteOffset];
            const std::size_t tng_bi_inc = tng_bv.byteStride > 0 ? tng_bv.byteStride : sizeof(math::Vec4<float>);
            std::size_t bi = 0;
            for (auto& vertex : vertices) {
                vertex.tangent = *reinterpret_cast<const math::Vec4<float>*>(&tng_b[bi]);
                bi += tng_bi_inc;
            }
        }

        {
            const auto* const txc_b = &bfs[txc_bv.buffer].data[txc_bv.byteOffset];
            const std::size_t txc_bi_inc = txc_bv.byteStride > 0 ? txc_bv.byteStride : sizeof(math::Vec2<float>);
            std::size_t bi = 0;
            for (auto& vertex : vertices) {
                vertex.uv = *reinterpret_cast<const math::Vec2<float>*>(&txc_b[bi]);
                bi += txc_bi_inc;
            }
        }

        if (is_animated)
        {
            {
                const auto* const bwt_b = &bfs[bwt_bv.buffer].data[bwt_bv.byteOffset];
                const std::size_t bwt_bi_inc = bwt_bv.byteStride > 0 ? bwt_bv.byteStride : sizeof(math::Vec4<float>);
                std::size_t bi = 0;
                for (auto& vertex : animated_vertices) {
                    vertex.bone_weights = *reinterpret_cast<const math::Vec4<float>*>(&bwt_b[bi]);
                    bi += bwt_bi_inc;
                }
            }

            {
                const auto* const bin_b = &bfs[bin_bv.buffer].data[bin_bv.byteOffset];
                const auto bin_bi_inc = [&]() noexcept -> std::size_t {
                    if (bin_bv.byteStride > 0)
                        return bin_bv.byteStride;
                    switch (bin_a.componentType) {
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                    case TINYGLTF_COMPONENT_TYPE_INT:
                        return 16;
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                    case TINYGLTF_COMPONENT_TYPE_SHORT:
                        return 8;
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                    case TINYGLTF_COMPONENT_TYPE_BYTE:
                        return 4;
                    default:
                        GX_UNEXPECTED;
                    }
                }();
                std::size_t bi = 0;
                switch (bin_a.componentType) {
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                    for (auto& vertex : animated_vertices) {
                        vertex.bone_indices = math::Vec4<float>(*reinterpret_cast<const math::Vec4<std::uint32_t>*>(&bin_b[bi])) + 0.1f;
                        bi += bin_bi_inc;
                    }
                    break;
                case TINYGLTF_COMPONENT_TYPE_INT:
                    for (auto& vertex : animated_vertices) {
                        vertex.bone_indices = math::Vec4<float>(*reinterpret_cast<const math::Vec4<std::int32_t>*>(&bin_b[bi])) + 0.1f;
                        bi += bin_bi_inc;
                    }
                    break;
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                    for (auto& vertex : animated_vertices) {
                        vertex.bone_indices = math::Vec4<float>(*reinterpret_cast<const math::Vec4<std::uint16_t>*>(&bin_b[bi])) + 0.1f;
                        bi += bin_bi_inc;
                    }
                    break;
                case TINYGLTF_COMPONENT_TYPE_SHORT:
                    for (auto& vertex : animated_vertices) {
                        vertex.bone_indices = math::Vec4<float>(*reinterpret_cast<const math::Vec4<std::int16_t>*>(&bin_b[bi])) + 0.1f;
                        bi += bin_bi_inc;
                    }
                    break;
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                    for (auto& vertex : animated_vertices) {
                        vertex.bone_indices = math::Vec4<float>(*reinterpret_cast<const math::Vec4<std::uint8_t>*>(&bin_b[bi])) + 0.1f;
                        bi += bin_bi_inc;
                    }
                    break;
                case TINYGLTF_COMPONENT_TYPE_BYTE:
                    for (auto& vertex : animated_vertices) {
                        vertex.bone_indices = math::Vec4<float>(*reinterpret_cast<const math::Vec4<std::int8_t>*>(&bin_b[bi])) + 0.1f;
                        bi += bin_bi_inc;
                    }
                    break;
                default:
                    GX_UNEXPECTED;
                }
            }
        }

        const auto* const ids_b = &bfs[ids_bv.buffer].data[ids_bv.byteOffset];
        const std::size_t ids_bi_inc = [&]() noexcept {
            if (ids_bv.byteStride > 0)
                return ids_bv.byteStride;
            switch (ids_a.componentType) {
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
            case TINYGLTF_COMPONENT_TYPE_INT:
                return sizeof(std::uint32_t);
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
            case TINYGLTF_COMPONENT_TYPE_SHORT:
                return sizeof(std::uint16_t);
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
            case TINYGLTF_COMPONENT_TYPE_BYTE:
                return sizeof(std::uint8_t);
            default:
                GX_UNEXPECTED;
            }
        }();
        std::size_t bi = 0;
        switch (ids_a.componentType) {
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
        case TINYGLTF_COMPONENT_TYPE_INT:
            for (auto& i : indices) {
                i = *reinterpret_cast<const std::uint32_t*>(&ids_b[bi]);
                bi += ids_bi_inc;
            }
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
        case TINYGLTF_COMPONENT_TYPE_SHORT:
            for (auto& i : indices) {
                i = *reinterpret_cast<const std::uint16_t*>(&ids_b[bi]);
                bi += ids_bi_inc;
            }
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
        case TINYGLTF_COMPONENT_TYPE_BYTE:
            for (auto& i : indices) {
                i = *reinterpret_cast<const std::uint16_t*>(&ids_b[bi]);
                bi += ids_bi_inc;
            }
            break;
        default:
            GX_UNEXPECTED;
        }

        for (std::size_t vi = 0; vi < animated_vertices.size(); ++vi)
            animated_vertices[vi].base = vertices[vi];

        if (is_animated)
            gx_mesh = e.get_mesh_manager()->build(
                std::string(msh.name),
                std::move(animated_vertices),
                std::move(indices),
                std::move(box),
                core::sync::EndCallerIgnored(mesh_end_callback));
        else
            gx_mesh = e.get_mesh_manager()->build(
                std::string(msh.name),
                std::move(vertices),
                std::move(indices),
                std::move(box),
                core::sync::EndCallerIgnored(mesh_end_callback));

        const tinygltf::Material& mat = data.materials[primitive.material];
        GX_LOG_D("Loading material: " << mat.name);
        if (mat.doubleSided)
            GX_UNIMPLEMENTED;
        const auto& albedo_factor = mat.pbrMetallicRoughness.baseColorFactor;
        GX_ASSERT(albedo_factor.size() == 4);
        gx_mat.get_alpha_cutoff_occlusion_strength().x = static_cast<float>(mat.alphaCutoff);
        if ("OPAQUE" == mat.alphaMode)
            gx_mat.set_alpha_mode(material::AlphaMode::Opaque);
        else if ("TRANSPARENT" == mat.alphaMode)
            gx_mat.set_alpha_mode(material::AlphaMode::Transparent);
        else
            GX_UNEXPECTED;
        GX_ASSERT(mat.emissiveFactor.size() == 3);
        gx_mat.get_emission_roughness_factor().x = static_cast<float>(mat.emissiveFactor[0]);
        gx_mat.get_emission_roughness_factor().y = static_cast<float>(mat.emissiveFactor[1]);
        gx_mat.get_emission_roughness_factor().z = static_cast<float>(mat.emissiveFactor[2]);
        gx_mat.set_emission(create_texture2d(mat.emissiveTexture.index, mesh_end_callback, gx_mat.get_emission()));
        gx_mat.set_normal(create_texture2d(mat.normalTexture.index, mesh_end_callback, gx_mat.get_normal()));
        gx_mat.get_normal_metallic_factor().x = static_cast<float>(mat.normalTexture.scale);
        gx_mat.get_normal_metallic_factor().y = static_cast<float>(mat.normalTexture.scale);
        gx_mat.get_normal_metallic_factor().z = 1.0f; // stange but tinygltf docs says that
        gx_mat.set_occlusion(create_texture2d(mat.occlusionTexture.index, mesh_end_callback, gx_mat.get_occlusion()));
        gx_mat.get_alpha_cutoff_occlusion_strength().y = static_cast<float>(mat.occlusionTexture.strength);
        gx_mat.set_albedo(create_texture2d(mat.pbrMetallicRoughness.baseColorTexture.index, mesh_end_callback, gx_mat.get_albedo()));
        gx_mat.get_albedo_factor() = math::Vec4<float>(
            static_cast<float>(albedo_factor[0]),
            static_cast<float>(albedo_factor[1]),
            static_cast<float>(albedo_factor[2]),
            static_cast<float>(albedo_factor[3]));
        gx_mat.set_metallic_roughness(create_texture2d(mat.pbrMetallicRoughness.metallicRoughnessTexture.index, mesh_end_callback, gx_mat.get_metallic_roughness()));
        gx_mat.get_emission_roughness_factor().w = static_cast<float>(mat.pbrMetallicRoughness.roughnessFactor);
        gx_mat.get_normal_metallic_factor().w = static_cast<float>(mat.pbrMetallicRoughness.metallicFactor);
        return gx_mesh;
    };

    const auto apply_transform = [&](const int node_index, physics::Transformation& transform) noexcept {
        const auto& node = data.nodes[node_index];
        const std::vector<double>& rotation = node.rotation;
        GX_ASSERT_D(rotation.size() == 0 || rotation.size() == 4);
        const std::vector<double>& scale = node.scale;
        GX_ASSERT_D(scale.size() == 0 || scale.size() == 3);
        const std::vector<double>& translation = node.translation;
        GX_ASSERT_D(translation.size() == 0 || translation.size() == 3);
        // const std::vector<double>& matrix = scene_node.matrix; // length must be 0 or 16
        // const std::vector<double>& weights = scene_node.weights;
        if (scale.size() == 3)
            transform.local_scale({ scale[0], scale[1], scale[2] });
        if (rotation.size() == 4)
            transform.local_rotate_quaternion(rotation[0], rotation[1], rotation[2], rotation[3]);
        if (translation.size() == 3)
            transform.set_location(translation[0], translation[1], translation[2]);
    };

    std::function<void(int, const core::sync::EndCallerIgnored&, render::scene::Builder&)> process_node;
    process_node = [&](const int node_index, const core::sync::EndCallerIgnored& node_end_callback, render::scene::Builder& scene_builder) noexcept {
        const auto& node = data.nodes[node_index];
        GX_LOG_D("Loading node: " << node.name);
        if (node.camera != -1) {
            GX_ASSERT_D(node.children.empty());
            const tinygltf::Camera& cmr = data.cameras[node.camera];
            GX_LOG_D("Loading camera: " << cmr.name);
            auto camera_builder = e.get_camera_manager()->build(cmr.name);
            auto& rnd_cmr = camera_builder->get_camera();
            if ("perspective" == cmr.type) {
                rnd_cmr.set_projection_type(camera::Projection::Perspective);
                rnd_cmr.set_far(static_cast<float>(cmr.perspective.zfar));
                rnd_cmr.set_near(static_cast<float>(cmr.perspective.znear));
                rnd_cmr.set_yfov(static_cast<float>(cmr.perspective.yfov));
            }
            else {
                rnd_cmr.set_projection_type(camera::Projection::Orthographic);
                rnd_cmr.set_far(static_cast<float>(cmr.orthographic.zfar));
                rnd_cmr.set_near(static_cast<float>(cmr.orthographic.znear));
            }
            apply_transform(node_index, camera_builder->get_transformation());
            scene_builder.add(std::move(camera_builder));
            return;
        }
        if (node.mesh != -1) {
            GX_ASSERT_D(node.children.empty());
            material::Pbr gx_mat(e, node_end_callback);
            auto gx_mesh = create_mesh(node.mesh, gx_mat, node_end_callback);
            auto model_builder = e.get_model_manager()->build(
                std::string(node.name),
                std::shared_ptr<gearoenix::render::mesh::Mesh>(gx_mesh),
                gearoenix::core::sync::EndCallerIgnored(node_end_callback),
                true);
            model_builder->set_material(gx_mat);
            if (node.skin != -1)
            {
                bones_memory.clear();
                bones_memory_indices.clear();
                node_bone_memory_index.clear();

                const auto& skin = data.skins[node.skin];
                for (const auto joint_node_index : skin.joints)
                {
                    const auto& bone_node = data.nodes[joint_node_index];
                    const auto bone_memory_index = physics::animation::Bone::construct(
                        bones_memory, bone_node.name,
                        static_cast<physics::animation::Bone::Index>(bone_node.children.size()));
                    bones_memory_indices.push_back(bone_memory_index);
                    node_bone_memory_index[joint_node_index] = bone_memory_index;
                    apply_transform(joint_node_index, physics::animation::Bone::get_transformation(bones_memory, bone_memory_index));
                }
                std::size_t bones_memory_indices_index = 0;
                for (const auto joint_node_index : skin.joints)
                {
                    const auto& bone_node = data.nodes[joint_node_index];
                    bone_children_indices.clear();
                    for (const auto child_index : bone_node.children)
                    {
                        bone_children_indices.push_back(node_bone_memory_index[child_index]);
                    }
                    const auto bone_memory_index = bones_memory_indices[bones_memory_indices_index];
                    ++bones_memory_indices_index;
                    physics::animation::Bone::set_children(bones_memory, bone_memory_index, bone_children_indices);
                }
                e.get_physics_engine()->get_animation_manager()->create_armature(
                    model_builder->get_entity_builder()->get_builder(),
                    skin.name,
                    bones_memory);
            }
            apply_transform(node_index, model_builder->get_transformation());
            scene_builder.add(std::move(model_builder));
            return;
        }
        constexpr auto light_ext_name = "KHR_lights_punctual";
        if (node.extensions.size() == 1 && node.extensions.begin()->first == light_ext_name) {
            const auto& light_ext = node.extensions.begin()->second;
            GX_ASSERT_D(light_ext.IsObject());
            const auto light_index = light_ext.Get("light").GetNumberAsInt();
            auto light_ext_search = data.extensions.find(light_ext_name);
            GX_ASSERT_D(data.extensions.end() != light_ext_search);
            const auto& lights = light_ext_search->second.Get("lights");
            GX_ASSERT_D(lights.IsArray());
            const auto& light_info = lights.Get(light_index);
            GX_ASSERT_D(light_info.IsObject());
            const auto& json_light_colour = light_info.Get("color");
            GX_ASSERT_D(json_light_colour.IsArray());
            GX_ASSERT_D(3 == json_light_colour.ArrayLen());
            GX_ASSERT_D(0.0 <= json_light_colour.Get(0).GetNumberAsDouble());
            GX_ASSERT_D(1.0 >= json_light_colour.Get(0).GetNumberAsDouble());
            GX_ASSERT_D(0.0 <= json_light_colour.Get(1).GetNumberAsDouble());
            GX_ASSERT_D(1.0 >= json_light_colour.Get(1).GetNumberAsDouble());
            GX_ASSERT_D(0.0 <= json_light_colour.Get(2).GetNumberAsDouble());
            GX_ASSERT_D(1.0 >= json_light_colour.Get(2).GetNumberAsDouble());
            const auto colour = math::Vec3<float>(
                static_cast<float>(json_light_colour.Get(0).GetNumberAsDouble()),
                static_cast<float>(json_light_colour.Get(1).GetNumberAsDouble()),
                static_cast<float>(json_light_colour.Get(2).GetNumberAsDouble()))
                * static_cast<float>(light_info.Get("intensity").GetNumberAsDouble());
            const auto light_type = light_info.Get("type").Get<std::string>();
            auto light_builder = e.get_light_manager()->build_shadow_caster_directional(
                node.name,
                1024,
                10.0f,
                1.0f,
                35.0f,
                end_callback);
            light_builder->get_light().colour = colour;
            apply_transform(node_index, light_builder->get_transformation());
            scene_builder.add(std::move(light_builder));
            return;
        }
        if (node.children.size() == 2)
        { // This is root armature
            GX_ASSERT_D(node.rotation.empty());
            GX_ASSERT_D(node.scale.empty());
            GX_ASSERT_D(node.translation.empty());
            auto mesh_node_index = node.children[0];
            if (data.nodes[mesh_node_index].mesh != -1 && data.nodes[mesh_node_index].skin != -1)
            {
                process_node(mesh_node_index, node_end_callback, scene_builder);
                return;
            }
            mesh_node_index = node.children[1];
            if (data.nodes[mesh_node_index].mesh != -1 && data.nodes[mesh_node_index].skin != -1)
            {
                process_node(mesh_node_index, node_end_callback, scene_builder);
                return;
            }
            GX_UNEXPECTED;
        }
        GX_UNEXPECTED;
    };

    for (const tinygltf::Scene& scn : data.scenes) {
        GX_LOG_D("Loading scene: " << scn.name);
        auto scene_builder = e.get_scene_manager()->build(scn.name);
        auto scene_end_callback = gearoenix::core::sync::EndCallerIgnored([scene_builder, end_callback = end_callback] {});
        for (const int scene_node_index : scn.nodes) {
            process_node(scene_node_index, scene_end_callback, *scene_builder);
        }
        result.push_back(std::move(scene_builder));
    }
    return result;
}
