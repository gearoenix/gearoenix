#include "gx-rnd-gltf-loader.hpp"
#include "../physics/animation/gx-phs-anm-bone.hpp"
#include "../physics/animation/gx-phs-anm-interpolation.hpp"
#include "../physics/animation/gx-phs-anm-manager.hpp"
#include "../physics/gx-phs-engine.hpp"
#include "../platform/stream/gx-plt-stm-path.hpp"
#include "camera/gx-rnd-cmr-builder.hpp"
#include "camera/gx-rnd-cmr-camera.hpp"
#include "camera/gx-rnd-cmr-manager.hpp"
#include "engine/gx-rnd-eng-engine.hpp"
#include "light/gx-rnd-lt-builder.hpp"
#include "light/gx-rnd-lt-light.hpp"
#include "light/gx-rnd-lt-manager.hpp"
#include "mesh/gx-rnd-msh-manager.hpp"
#include "mesh/gx-rnd-msh-mesh.hpp"
#include "model/gx-rnd-mdl-builder.hpp"
#include "model/gx-rnd-mdl-manager.hpp"
#include "scene/gx-rnd-scn-builder.hpp"
#include "scene/gx-rnd-scn-manager.hpp"
#include "texture/gx-rnd-txt-manager.hpp"
#include "texture/gx-rnd-txt-texture-2d.hpp"

#define TINYGLTF_NOEXCEPTION
#define TINYGLTF_IMPLEMENTATION
#include <tinygltf/tiny_gltf.h>

[[nodiscard]] static bool is_gltf_node_a_bone(const tinygltf::Node& n) noexcept
{
    return n.skin == -1 && n.mesh == -1 && n.camera == -1 && n.extensions.empty();
}

[[nodiscard]] static const std::shared_ptr<gearoenix::render::texture::Texture2D>& create_texture2d(
    const int index,
    const gearoenix::core::sync::EndCallerIgnored& txt_end_callback,
    const std::shared_ptr<gearoenix::render::texture::Texture2D>& default_value,
    std::vector<std::shared_ptr<gearoenix::render::texture::Texture2D>>& gx_txt2ds,
    const tinygltf::Model& data,
    gearoenix::render::engine::Engine& e) noexcept
{
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
            return gearoenix::render::texture::Filter::Linear;
        case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR:
            needs_mipmap = true;
            return gearoenix::render::texture::Filter::LinearMipmapLinear;
        case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST:
            needs_mipmap = true;
            return gearoenix::render::texture::Filter::LinearMipmapNearest;
        case TINYGLTF_TEXTURE_FILTER_NEAREST:
            return gearoenix::render::texture::Filter::Nearest;
        case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR:
            needs_mipmap = true;
            return gearoenix::render::texture::Filter::NearestMipmapLinear;
        case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST:
            needs_mipmap = true;
            return gearoenix::render::texture::Filter::NearestMipmapNearest;
        default:
            GX_UNEXPECTED;
        }
    };
    const auto convert_wrap = [&](const int w) {
        switch (w) {
        case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE:
            return gearoenix::render::texture::Wrap::ClampToEdge;
        case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT:
            return gearoenix::render::texture::Wrap::Mirror;
        case TINYGLTF_TEXTURE_WRAP_REPEAT:
            return gearoenix::render::texture::Wrap::Repeat;
        default:
            GX_UNEXPECTED;
        }
    };
    const gearoenix::render::texture::TextureInfo txt_info {
        .format = gearoenix::render::texture::TextureFormat::Unknown, // [&] {
        // if (2 == img.component && 8 == img.bits && TINYGLTF_COMPONENT_TYPE_BYTE == img.pixel_type) return texture::TextureFormat::RgbaUint8;
        // if (2 == img.component && 8 == img.bits && TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE == img.pixel_type) return texture::TextureFormat::RgbaUint8;
        // if (3 == img.component && 8 == img.bits && TINYGLTF_COMPONENT_TYPE_BYTE == img.pixel_type) return texture::TextureFormat::RgbaUint8;
        // if (3 == img.component && 8 == img.bits && TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE == img.pixel_type) return texture::TextureFormat::RgbaUint8;
        // if (4 == img.component && 8 == img.bits && TINYGLTF_COMPONENT_TYPE_BYTE == img.pixel_type) return texture::TextureFormat::RgbaUint8;
        // if (4 == img.component && 8 == img.bits && TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE == img.pixel_type) return texture::TextureFormat::RgbaUint8;
        // if (4 == img.component && 32 == img.bits && TINYGLTF_COMPONENT_TYPE_FLOAT == img.pixel_type) return texture::TextureFormat::RgbaFloat32;
        // GX_UNIMPLEMENTED;
        //}(),
        .sampler_info = gearoenix::render::texture::SamplerInfo {
            .min_filter = convert_filter(smp.minFilter),
            .mag_filter = convert_filter(smp.magFilter),
            .wrap_s = convert_wrap(smp.wrapS),
            .wrap_t = convert_wrap(smp.wrapT),
        },
        .width = 0, // static_cast<std::size_t>(img.width),
        .height = 0, // static_cast<std::size_t>(img.height),
        .depth = 0,
        .type = gearoenix::render::texture::Type::Unknown, // texture::Type::Texture2D,
        .has_mipmap = needs_mipmap,
    };
    gx_txt = e.get_texture_manager()->create_2d_from_formatted(txt.name, img_ptr, img_sz, txt_info, txt_end_callback);
    return gx_txt;
}

[[nodiscard]] static const std::shared_ptr<gearoenix::render::mesh::Mesh>& create_mesh(
    const int index,
    gearoenix::render::material::Pbr& gx_mat,
    const gearoenix::core::sync::EndCallerIgnored& mesh_end_callback,
    std::vector<std::shared_ptr<gearoenix::render::mesh::Mesh>>& gx_meshes,
    const tinygltf::Model& data,
    gearoenix::render::engine::Engine& e,
    std::vector<std::shared_ptr<gearoenix::render::texture::Texture2D>>& gx_txt2ds,
    std::vector<int> bone_index_map) noexcept
{
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

    gearoenix::math::Aabb3<double> box(
        gearoenix::math::Vec3<double>(pos_max[0], pos_max[1], pos_max[2]),
        gearoenix::math::Vec3<double>(pos_min[0], pos_min[1], pos_min[2]));

    std::vector<gearoenix::render::PbrVertex> vertices(pos_a.count);
    std::vector<gearoenix::render::PbrVertexAnimated> animated_vertices(is_animated ? pos_a.count : 0);
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
        const std::size_t pos_bi_inc = pos_bv.byteStride > 0 ? pos_bv.byteStride : sizeof(gearoenix::math::Vec3<float>);
        std::size_t bi = 0;
        for (auto& vertex : vertices) {
            vertex.position = *reinterpret_cast<const gearoenix::math::Vec3<float>*>(&pos_b[bi]);
            bi += pos_bi_inc;
        }
    }

    {
        const auto* const nrm_b = &bfs[nrm_bv.buffer].data[nrm_bv.byteOffset];
        const std::size_t nrm_bi_inc = nrm_bv.byteStride > 0 ? nrm_bv.byteStride : sizeof(gearoenix::math::Vec3<float>);
        std::size_t bi = 0;
        for (auto& vertex : vertices) {
            vertex.normal = *reinterpret_cast<const gearoenix::math::Vec3<float>*>(&nrm_b[bi]);
            bi += nrm_bi_inc;
        }
    }

    {
        const auto* const tng_b = &bfs[tng_bv.buffer].data[tng_bv.byteOffset];
        const std::size_t tng_bi_inc = tng_bv.byteStride > 0 ? tng_bv.byteStride : sizeof(gearoenix::math::Vec4<float>);
        std::size_t bi = 0;
        for (auto& vertex : vertices) {
            vertex.tangent = *reinterpret_cast<const gearoenix::math::Vec4<float>*>(&tng_b[bi]);
            bi += tng_bi_inc;
        }
    }

    {
        const auto* const txc_b = &bfs[txc_bv.buffer].data[txc_bv.byteOffset];
        const std::size_t txc_bi_inc = txc_bv.byteStride > 0 ? txc_bv.byteStride : sizeof(gearoenix::math::Vec2<float>);
        std::size_t bi = 0;
        for (auto& vertex : vertices) {
            vertex.uv = *reinterpret_cast<const gearoenix::math::Vec2<float>*>(&txc_b[bi]);
            bi += txc_bi_inc;
        }
    }

    if (is_animated) {
        {
            const auto* const bwt_b = &bfs[bwt_bv.buffer].data[bwt_bv.byteOffset];
            const std::size_t bwt_bi_inc = bwt_bv.byteStride > 0 ? bwt_bv.byteStride : sizeof(gearoenix::math::Vec4<float>);
            std::size_t bi = 0;
            for (auto& vertex : animated_vertices) {
                vertex.bone_weights = *reinterpret_cast<const gearoenix::math::Vec4<float>*>(&bwt_b[bi]);
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
            const auto convert_indices = [&]<typename T>() noexcept {
                auto is = gearoenix::math::Vec4<int>(*reinterpret_cast<const gearoenix::math::Vec4<T>*>(&bin_b[bi]));
                is.x = bone_index_map[is.x];
                is.y = bone_index_map[is.y];
                is.z = bone_index_map[is.z];
                is.w = bone_index_map[is.w];
                return gearoenix::math::Vec4<float>(is) + 0.001f;
            };
            switch (bin_a.componentType) {
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                for (auto& vertex : animated_vertices) {
                    vertex.bone_indices = convert_indices.operator()<std::uint32_t>();
                    bi += bin_bi_inc;
                }
                break;
            case TINYGLTF_COMPONENT_TYPE_INT:
                for (auto& vertex : animated_vertices) {
                    vertex.bone_indices = convert_indices.operator()<std::int32_t>();
                    bi += bin_bi_inc;
                }
                break;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                for (auto& vertex : animated_vertices) {
                    vertex.bone_indices = convert_indices.operator()<std::uint16_t>();
                    bi += bin_bi_inc;
                }
                break;
            case TINYGLTF_COMPONENT_TYPE_SHORT:
                for (auto& vertex : animated_vertices) {
                    vertex.bone_indices = convert_indices.operator()<std::int16_t>();
                    bi += bin_bi_inc;
                }
                break;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                for (auto& vertex : animated_vertices) {
                    vertex.bone_indices = convert_indices.operator()<std::uint8_t>();
                    bi += bin_bi_inc;
                }
                break;
            case TINYGLTF_COMPONENT_TYPE_BYTE:
                for (auto& vertex : animated_vertices) {
                    vertex.bone_indices = convert_indices.operator()<std::int8_t>();
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
            gearoenix::core::sync::EndCallerIgnored(mesh_end_callback));
    else
        gx_mesh = e.get_mesh_manager()->build(
            std::string(msh.name),
            std::move(vertices),
            std::move(indices),
            std::move(box),
            gearoenix::core::sync::EndCallerIgnored(mesh_end_callback));

    const tinygltf::Material& mat = data.materials[primitive.material];
    GX_LOG_D("Loading material: " << mat.name);
    if (mat.doubleSided)
        GX_UNIMPLEMENTED;
    const auto& albedo_factor = mat.pbrMetallicRoughness.baseColorFactor;
    GX_ASSERT(albedo_factor.size() == 4);
    gx_mat.get_alpha_cutoff_occlusion_strength().x = static_cast<float>(mat.alphaCutoff);
    if ("OPAQUE" == mat.alphaMode)
        gx_mat.set_alpha_mode(gearoenix::render::material::AlphaMode::Opaque);
    else if ("TRANSPARENT" == mat.alphaMode)
        gx_mat.set_alpha_mode(gearoenix::render::material::AlphaMode::Transparent);
    else
        GX_UNEXPECTED;
    GX_ASSERT(mat.emissiveFactor.size() == 3);
    gx_mat.get_emission_roughness_factor().x = static_cast<float>(mat.emissiveFactor[0]);
    gx_mat.get_emission_roughness_factor().y = static_cast<float>(mat.emissiveFactor[1]);
    gx_mat.get_emission_roughness_factor().z = static_cast<float>(mat.emissiveFactor[2]);
    gx_mat.set_emission(create_texture2d(mat.emissiveTexture.index, mesh_end_callback, gx_mat.get_emission(), gx_txt2ds, data, e));
    gx_mat.set_normal(create_texture2d(mat.normalTexture.index, mesh_end_callback, gx_mat.get_normal(), gx_txt2ds, data, e));
    gx_mat.get_normal_metallic_factor().x = static_cast<float>(mat.normalTexture.scale);
    gx_mat.get_normal_metallic_factor().y = static_cast<float>(mat.normalTexture.scale);
    gx_mat.get_normal_metallic_factor().z = 1.0f; // strange but tinygltf docs says that
    gx_mat.set_occlusion(create_texture2d(mat.occlusionTexture.index, mesh_end_callback, gx_mat.get_occlusion(), gx_txt2ds, data, e));
    gx_mat.get_alpha_cutoff_occlusion_strength().y = static_cast<float>(mat.occlusionTexture.strength);
    gx_mat.set_albedo(create_texture2d(mat.pbrMetallicRoughness.baseColorTexture.index, mesh_end_callback, gx_mat.get_albedo(), gx_txt2ds, data, e));
    gx_mat.get_albedo_factor() = gearoenix::math::Vec4<float>(
        static_cast<float>(albedo_factor[0]),
        static_cast<float>(albedo_factor[1]),
        static_cast<float>(albedo_factor[2]),
        static_cast<float>(albedo_factor[3]));
    gx_mat.set_metallic_roughness(create_texture2d(mat.pbrMetallicRoughness.metallicRoughnessTexture.index, mesh_end_callback, gx_mat.get_metallic_roughness(), gx_txt2ds, data, e));
    gx_mat.get_emission_roughness_factor().w = static_cast<float>(mat.pbrMetallicRoughness.roughnessFactor);
    gx_mat.get_normal_metallic_factor().w = static_cast<float>(mat.pbrMetallicRoughness.metallicFactor);
    return gx_mesh;
}

static void apply_transform(
    const int node_index,
    gearoenix::physics::Transformation& transform,
    const tinygltf::Model& data) noexcept
{
    const auto& node = data.nodes[node_index];
    const std::vector<double>& rotation = node.rotation;
    GX_ASSERT_D(rotation.empty() || rotation.size() == 4);
    const std::vector<double>& scale = node.scale;
    GX_ASSERT_D(scale.empty() || scale.size() == 3);
    const std::vector<double>& translation = node.translation;
    GX_ASSERT_D(translation.empty() || translation.size() == 3);
    if (scale.size() == 3)
        transform.local_scale({ scale[0], scale[1], scale[2] });
    if (rotation.size() == 4)
        transform.local_rotate(gearoenix::math::Quat<double>(rotation[0], rotation[1], rotation[2], rotation[3]));
    if (translation.size() == 3)
        transform.set_local_location(gearoenix::math::Vec3<double>(translation[0], translation[1], translation[2]));
}

static void load_bone_info(
    gearoenix::physics::animation::BoneInfo& bone_info,
    const int bone_node_index,
    const std::string& parent_name,
    const tinygltf::Model& data,
    const std::map<std::string, int>& name_to_joint_index,
    const gearoenix::math::Mat4x4<float>* inv_bind_mats) noexcept
{
    const tinygltf::Node& bone_node = data.nodes[bone_node_index];
    bone_info.name = bone_node.name;
    bone_info.parent_name = parent_name;
    {
        const auto search = name_to_joint_index.find(bone_node.name);
        GX_ASSERT_D(name_to_joint_index.end() != search);
        bone_info.inverse_bind = inv_bind_mats[search->second];
    }
    apply_transform(bone_node_index, bone_info.transform, data);
    bone_info.children.resize(bone_node.children.size());
    for (std::size_t child_index = 0; child_index < bone_node.children.size(); ++child_index) {
        load_bone_info(bone_info.children[child_index], bone_node.children[child_index], bone_node.name, data, name_to_joint_index, inv_bind_mats);
    }
}

static void set_bone_map(
    const gearoenix::physics::animation::BoneInfo& bone_info,
    const std::map<std::string, int>& name_to_joint_index,
    std::vector<int>& bone_index_map,
    int& index) noexcept
{
    for (const auto& child : bone_info.children) {
        const auto search = name_to_joint_index.find(child.name);
        GX_ASSERT_D(name_to_joint_index.end() != search);
        ++index;
        bone_index_map[search->second] = index;
    }
    for (const auto& child : bone_info.children) {
        set_bone_map(child, name_to_joint_index, bone_index_map, index);
    }
}

static std::pair<std::vector<int>, std::optional<gearoenix::physics::animation::BoneInfo>> process_skin(
    const tinygltf::Node& node,
    const tinygltf::Model& data) noexcept
{
    if (node.skin == -1)
        return std::make_pair(std::vector<int>(), std::nullopt);
    const auto& skin = data.skins[node.skin];
    std::vector<int> bone_index_map(skin.joints.size());
    GX_ASSERT_D(-1 != skin.inverseBindMatrices);
    const auto& inv_bind_mat_acc = data.accessors[skin.inverseBindMatrices];
    GX_ASSERT_D(inv_bind_mat_acc.count == skin.joints.size());
    GX_ASSERT_D(inv_bind_mat_acc.type == TINYGLTF_TYPE_MAT4);
    GX_ASSERT_D(inv_bind_mat_acc.bufferView != -1);
    const auto& inv_bind_mat_bv = data.bufferViews[inv_bind_mat_acc.bufferView];
    GX_ASSERT_D(inv_bind_mat_bv.byteLength == inv_bind_mat_acc.count * sizeof(gearoenix::math::Mat4x4<float>));
    GX_ASSERT_D(inv_bind_mat_bv.byteStride == 0);
    const auto inv_bind_mats = reinterpret_cast<const gearoenix::math::Mat4x4<float>*>(
        &data.buffers[inv_bind_mat_bv.buffer].data[inv_bind_mat_bv.byteOffset]);
    std::set<int> root_bones_nodes;
    std::map<std::string, int> name_to_joint_index;
    int joint_index = 0;
    for (const auto bone_node_index : skin.joints) {
        root_bones_nodes.emplace(bone_node_index);
        name_to_joint_index[data.nodes[bone_node_index].name] = joint_index;
        ++joint_index;
    }
    for (const auto bone_node_index : skin.joints) {
        const auto& bone_node = data.nodes[bone_node_index];
        for (const auto child_node_index : bone_node.children) {
            root_bones_nodes.erase(child_node_index);
        }
    }
    GX_ASSERT(root_bones_nodes.size() == 1);
    gearoenix::physics::animation::BoneInfo bones_info;
    load_bone_info(bones_info, *root_bones_nodes.begin(), "", data, name_to_joint_index, inv_bind_mats);
    joint_index = 0;
    {
        const auto search = name_to_joint_index.find(bones_info.name);
        GX_ASSERT_D(name_to_joint_index.end() != search);
        bone_index_map[search->second] = joint_index;
    }
    set_bone_map(bones_info, name_to_joint_index, bone_index_map, joint_index);
    return { std::move(bone_index_map), std::move(bones_info) };
}

static void process_node(
    const int node_index,
    const gearoenix::core::sync::EndCallerIgnored& node_end_callback,
    gearoenix::render::scene::Builder& scene_builder,
    const tinygltf::Model& data,
    gearoenix::render::engine::Engine& e,
    std::vector<std::shared_ptr<gearoenix::render::mesh::Mesh>>& gx_meshes,
    std::vector<std::shared_ptr<gearoenix::render::texture::Texture2D>>& gx_txt2ds,
    const std::map<int, gearoenix::physics::animation::BoneChannelBuilder>& bones_channels) noexcept
{
    const auto& node = data.nodes[node_index];
    GX_LOG_D("Loading node: " << node.name);
    if (node.camera != -1) {
        GX_ASSERT_D(node.children.empty());
        const tinygltf::Camera& cmr = data.cameras[node.camera];
        GX_LOG_D("Loading camera: " << cmr.name);
        auto camera_builder = e.get_camera_manager()->build(cmr.name);
        auto& rnd_cmr = camera_builder->get_camera();
        if ("perspective" == cmr.type) {
            rnd_cmr.set_projection_type(gearoenix::render::camera::Projection::Perspective);
            rnd_cmr.set_far(static_cast<float>(cmr.perspective.zfar));
            rnd_cmr.set_near(static_cast<float>(cmr.perspective.znear));
            rnd_cmr.set_yfov(static_cast<float>(cmr.perspective.yfov));
        } else {
            rnd_cmr.set_projection_type(gearoenix::render::camera::Projection::Orthographic);
            rnd_cmr.set_far(static_cast<float>(cmr.orthographic.zfar));
            rnd_cmr.set_near(static_cast<float>(cmr.orthographic.znear));
        }
        auto& transform = camera_builder->get_transformation();
        apply_transform(node_index, transform, data);
        scene_builder.add(std::move(camera_builder));
        return;
    }
    if (node.mesh != -1) {
        auto [bone_index_map, bones_info] = process_skin(node, data);
        GX_ASSERT_D(node.children.empty());
        gearoenix::render::material::Pbr gx_mat(e, node_end_callback);
        auto gx_mesh = create_mesh(node.mesh, gx_mat, node_end_callback, gx_meshes, data, e, gx_txt2ds, bone_index_map);
        auto model_builder = e.get_model_manager()->build(
            std::string(node.name),
            std::move(gx_mesh),
            gearoenix::core::sync::EndCallerIgnored(node_end_callback),
            true);
        model_builder->set_material(gx_mat);
        if (bone_index_map.size() > 1) {
            e.get_physics_engine()->get_animation_manager()->create_armature(
                model_builder->get_entity_builder()->get_builder(),
                bones_info.value());
            const auto& skin = data.skins[node.skin];
            gearoenix::physics::animation::ArmatureAnimationInfo armature_animation_info;
            for (const auto bone_node_index : skin.joints) {
                auto bone_channel_search = bones_channels.find(bone_node_index);
                GX_ASSERT(bones_channels.end() != bone_channel_search);
                armature_animation_info.channels.push_back(bone_channel_search->second);
            }
            armature_animation_info.optimise();
            e.get_physics_engine()->get_animation_manager()->create_animation_player(
                model_builder->get_entity_builder()->get_builder(),
                armature_animation_info);
        }
        apply_transform(node_index, model_builder->get_transformation(), data);
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
        const auto colour = gearoenix::math::Vec3<float>(
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
            node_end_callback);
        light_builder->get_light().colour = colour;
        auto& transform = light_builder->get_transformation();
        apply_transform(node_index, transform, data);
        scene_builder.add(std::move(light_builder));
        return;
    }
    if (node.children.size() == 2) { // This is root armature
        GX_ASSERT_D(node.rotation.empty());
        GX_ASSERT_D(node.scale.empty());
        GX_ASSERT_D(node.translation.empty());
        auto mesh_node_index = node.children[0];
        if (data.nodes[mesh_node_index].mesh != -1 && data.nodes[mesh_node_index].skin != -1) {
            process_node(mesh_node_index, node_end_callback, scene_builder, data, e, gx_meshes, gx_txt2ds, bones_channels);
            return;
        }
        mesh_node_index = node.children[1];
        if (data.nodes[mesh_node_index].mesh != -1 && data.nodes[mesh_node_index].skin != -1) {
            process_node(mesh_node_index, node_end_callback, scene_builder, data, e, gx_meshes, gx_txt2ds, bones_channels);
            return;
        }
        GX_LOG_F("The correct node for skin and mesh not found. Node: " << node.name);
    }
    GX_LOG_F("Unexpected node in the scene nodes. Node: " << node.name);
}

template <template <typename> typename Value>
static void read_output(
    std::vector<std::pair<double, gearoenix::physics::animation::Keyframe<Value<double>>>>& keyframes,
    const tinygltf::Accessor& input,
    const tinygltf::Accessor& output,
    const tinygltf::BufferView& output_bv,
    const gearoenix::physics::animation::Interpolation interpolation,
    const std::size_t output_b_ptr,
    const std::vector<float>& times) noexcept
{
    keyframes.reserve(input.count);
    GX_ASSERT_D(output.count * sizeof(Value<float>) == output_bv.byteLength);
    switch (interpolation) {
    case gearoenix::physics::animation::Interpolation::Gltf2CubicSPLine: {
        GX_ASSERT_D(input.count * sizeof(Value<float>) * 3 == output_bv.byteLength);
        for (std::size_t data_i = 0, curr_output_ptr = output_b_ptr;
             data_i < input.count; ++data_i, curr_output_ptr += sizeof(Value<float>)) {
            const std::size_t in_ptr = curr_output_ptr;
            const auto in = Value<double>(*reinterpret_cast<const Value<float>*>(in_ptr));
            curr_output_ptr += sizeof(Value<float>);
            const std::size_t key_ptr = curr_output_ptr;
            const auto key = Value<double>(*reinterpret_cast<const Value<float>*>(key_ptr));
            curr_output_ptr += sizeof(Value<float>);
            const std::size_t out_ptr = curr_output_ptr;
            const auto out = Value<double>(*reinterpret_cast<const Value<float>*>(out_ptr));
            keyframes.emplace_back(
                times[data_i],
                gearoenix::physics::animation::Keyframe<Value<double>> {
                    gearoenix::physics::animation::KeyframeGltf2Bezier<Value<double>> {
                        .key = key,
                        .in = in,
                        .out = out,
                    } });
        }
        break;
    }
    case gearoenix::physics::animation::Interpolation::Linear: {
        GX_ASSERT_D(input.count * sizeof(Value<float>) == output_bv.byteLength);
        for (std::size_t data_i = 0, curr_output_ptr = output_b_ptr;
             data_i < input.count; ++data_i, curr_output_ptr += sizeof(Value<float>)) {
            keyframes.emplace_back(
                times[data_i],
                gearoenix::physics::animation::KeyframeLinear<Value<double>> {
                    Value<double>(*reinterpret_cast<const Value<float>*>(curr_output_ptr)) });
        }
        break;
    }
    case gearoenix::physics::animation::Interpolation::Step: {
        GX_ASSERT_D(input.count * sizeof(Value<float>) == output_bv.byteLength);
        for (std::size_t data_i = 0, curr_output_ptr = output_b_ptr;
             data_i < input.count; ++data_i, curr_output_ptr += sizeof(Value<float>)) {
            keyframes.emplace_back(
                times[data_i],
                gearoenix::physics::animation::KeyframeStep<Value<double>> {
                    Value<double>(*reinterpret_cast<const Value<float>*>(curr_output_ptr)) });
        }
        break;
    }
    }
}

[[nodiscard]] static gearoenix::physics::animation::Interpolation convert_interpolation(const std::string& interpolation) noexcept
{
    if ("CUBICSPLINE" == interpolation)
        return gearoenix::physics::animation::Interpolation::Gltf2CubicSPLine;
    if ("LINEAR" == interpolation)
        return gearoenix::physics::animation::Interpolation::Linear;
    if ("STEP" == interpolation)
        return gearoenix::physics::animation::Interpolation::Step;
    GX_LOG_F("Unexpected interpolation " << interpolation);
}

std::vector<std::shared_ptr<gearoenix::render::scene::Builder>> gearoenix::render::load_gltf(
    engine::Engine& e,
    const platform::stream::Path& file,
    const core::sync::EndCallerIgnored& end_callback) noexcept
{
    tinygltf::TinyGLTF context;
    tinygltf::Model data;

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
    std::vector<std::shared_ptr<mesh::Mesh>> gx_meshes(data.meshes.size());

    std::map<int /*bone-node-index*/, physics::animation::BoneChannelBuilder> bones_channels;
    for (const tinygltf::Animation& anm : data.animations) {
        for (const tinygltf::AnimationChannel& chn : anm.channels) {
            const auto& bone_node = data.nodes[chn.target_node];
            GX_ASSERT(is_gltf_node_a_bone(bone_node));
            physics::animation::BoneChannelBuilder& bone_channels = bones_channels[chn.target_node];
            bone_channels.target_bone = bone_node.name;
            const tinygltf::AnimationSampler& smp = anm.samplers[chn.sampler];
            const auto interpolation = convert_interpolation(smp.interpolation);
            const tinygltf::Accessor& input = data.accessors[smp.input];
            const tinygltf::Accessor& output = data.accessors[smp.output];
            GX_ASSERT(input.type == TINYGLTF_TYPE_SCALAR);
            GX_ASSERT_D(input.count > 0);
            GX_ASSERT_D(output.count > 0);
            GX_ASSERT_D((interpolation != physics::animation::Interpolation::Gltf2CubicSPLine && input.count == output.count) || (interpolation == physics::animation::Interpolation::Gltf2CubicSPLine && (input.count * 3) == output.count));
            std::vector<float> times(input.count);
            const tinygltf::BufferView& input_bv = data.bufferViews[input.bufferView];
            const tinygltf::BufferView& output_bv = data.bufferViews[output.bufferView];
            GX_ASSERT_D(input.count * sizeof(float) == input_bv.byteLength);
            GX_ASSERT_D(0 == input_bv.byteStride);
            GX_ASSERT_D(0 == output_bv.byteStride);
            std::memcpy(times.data(), &data.buffers[input_bv.buffer].data[input_bv.byteOffset], input_bv.byteLength);
            const std::vector<std::uint8_t>& output_b = data.buffers[output_bv.buffer].data;
            const auto output_b_ptr = reinterpret_cast<std::size_t>(&output_b[output_bv.byteOffset]);
            if ("translation" == chn.target_path) {
                GX_ASSERT(output.type == TINYGLTF_TYPE_VEC3);
                read_output(bone_channels.translation_samples, input, output, output_bv, interpolation, output_b_ptr, times);
            } else if ("rotation" == chn.target_path) {
                GX_ASSERT(output.type == TINYGLTF_TYPE_VEC4);
                read_output(bone_channels.rotation_samples, input, output, output_bv, interpolation, output_b_ptr, times);
            } else if ("scale" == chn.target_path) {
                GX_ASSERT(output.type == TINYGLTF_TYPE_VEC3);
                read_output(bone_channels.scale_samples, input, output, output_bv, interpolation, output_b_ptr, times);
            } else
                GX_UNEXPECTED;
        }
    }

    for (const tinygltf::Scene& scn : data.scenes) {
        GX_LOG_D("Loading scene: " << scn.name);
        auto scene_builder = e.get_scene_manager()->build(scn.name);
        auto scene_end_callback = gearoenix::core::sync::EndCallerIgnored([scene_builder, end_callback = end_callback] {});
        for (const int scene_node_index : scn.nodes) {
            process_node(scene_node_index, scene_end_callback, *scene_builder, data, e, gx_meshes, gx_txt2ds, bones_channels);
        }
        result.push_back(std::move(scene_builder));
    }
    return result;
}
