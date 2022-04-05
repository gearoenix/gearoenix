#include "gx-rnd-gltf-loader.hpp"
#include "../physics/gx-phs-transformation.hpp"
#include "../platform/gx-plt-log.hpp"
#include "../platform/stream/gx-plt-stm-path.hpp"
#include "../platform/stream/gx-plt-stm-stream.hpp"
#include "camera/gx-rnd-cmr-builder.hpp"
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

#define TINYGLTF_NOEXCEPTION
#define TINYGLTF_IMPLEMENTATION
#include <tiny_gltf.h>

std::vector<gearoenix::core::ecs::Entity::id_t> gearoenix::render::load_gltf(
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

    std::vector<core::ecs::Entity::id_t> result;
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
        const auto& attrs = primitive.attributes;
        GX_ASSERT(attrs.size() == 4);
        for (const auto& att : attrs) {
            if ("POSITION" == att.first) {
                pos_ai = static_cast<std::size_t>(att.second);
            } else if ("NORMAL" == att.first) {
                nrm_ai = static_cast<std::size_t>(att.second);
            } else if ("TANGENT" == att.first) {
                tng_ai = static_cast<std::size_t>(att.second);
            } else if ("TEXCOORD_0" == att.first) {
                txc_ai = static_cast<std::size_t>(att.second);
            } else
                GX_UNEXPECTED;
        }
        GX_ASSERT(pos_ai != static_cast<std::size_t>(-1));
        GX_ASSERT(nrm_ai != static_cast<std::size_t>(-1));
        GX_ASSERT(tng_ai != static_cast<std::size_t>(-1));
        GX_ASSERT(txc_ai != static_cast<std::size_t>(-1));

        GX_ASSERT(-1 != primitive.indices);
        GX_ASSERT(-1 != primitive.material);

        const auto& pos_a = acs[pos_ai];
        const auto& nrm_a = acs[nrm_ai];
        const auto& tng_a = acs[tng_ai];
        const auto& txc_a = acs[txc_ai];
        const auto& ids_a = acs[primitive.indices];

        GX_CHECK_EQUAL(pos_a.count, nrm_a.count);
        GX_CHECK_EQUAL(pos_a.count, tng_a.count);
        GX_CHECK_EQUAL(pos_a.count, txc_a.count);

        GX_CHECK_EQUAL(pos_a.type, TINYGLTF_TYPE_VEC3);
        GX_CHECK_EQUAL(nrm_a.type, TINYGLTF_TYPE_VEC3);
        GX_CHECK_EQUAL(tng_a.type, TINYGLTF_TYPE_VEC4);
        GX_CHECK_EQUAL(txc_a.type, TINYGLTF_TYPE_VEC2);
        GX_CHECK_EQUAL(ids_a.type, TINYGLTF_TYPE_SCALAR);

        GX_CHECK_EQUAL(pos_a.componentType, TINYGLTF_COMPONENT_TYPE_FLOAT);
        GX_CHECK_EQUAL(nrm_a.componentType, TINYGLTF_COMPONENT_TYPE_FLOAT);
        GX_CHECK_EQUAL(tng_a.componentType, TINYGLTF_COMPONENT_TYPE_FLOAT);
        GX_CHECK_EQUAL(txc_a.componentType, TINYGLTF_COMPONENT_TYPE_FLOAT);

        const auto& pos_max = pos_a.maxValues;
        const auto& pos_min = pos_a.maxValues;
        GX_CHECK_EQUAL(pos_max.size(), 3);
        GX_CHECK_EQUAL(pos_min.size(), 3);

        math::Aabb3<double> box(
            math::Vec3<double>(pos_max[0], pos_max[1], pos_max[2]),
            math::Vec3<double>(pos_min[0], pos_min[1], pos_min[2]));

        std::vector<PbrVertex> vertices(pos_a.count);
        std::vector<std::uint32_t> indices(ids_a.count);

        const auto& pos_bv = bvs[pos_a.bufferView];
        const auto& nrm_bv = bvs[nrm_a.bufferView];
        const auto& tng_bv = bvs[tng_a.bufferView];
        const auto& txc_bv = bvs[txc_a.bufferView];
        const auto& ids_bv = bvs[ids_a.bufferView];

        const auto* const pos_b = &bfs[pos_bv.buffer].data[pos_bv.byteOffset];
        const std::size_t pos_bi_inc = pos_bv.byteStride > 0 ? pos_bv.byteStride : sizeof(math::Vec3<float>);
        std::size_t bi = 0;
        for (auto& vertex : vertices) {
            vertex.position = *reinterpret_cast<const math::Vec3<float>*>(&pos_b[bi]);
            bi += pos_bi_inc;
        }

        const auto* const nrm_b = &bfs[nrm_bv.buffer].data[nrm_bv.byteOffset];
        const std::size_t nrm_bi_inc = nrm_bv.byteStride > 0 ? nrm_bv.byteStride : sizeof(math::Vec3<float>);
        bi = 0;
        for (auto& vertex : vertices) {
            vertex.normal = *reinterpret_cast<const math::Vec3<float>*>(&nrm_b[bi]);
            bi += nrm_bi_inc;
        }

        const auto* const tng_b = &bfs[tng_bv.buffer].data[tng_bv.byteOffset];
        const std::size_t tng_bi_inc = tng_bv.byteStride > 0 ? tng_bv.byteStride : sizeof(math::Vec4<float>);
        bi = 0;
        for (auto& vertex : vertices) {
            vertex.tangent = *reinterpret_cast<const math::Vec4<float>*>(&tng_b[bi]);
            bi += tng_bi_inc;
        }

        const auto* const txc_b = &bfs[txc_bv.buffer].data[txc_bv.byteOffset];
        const std::size_t txc_bi_inc = txc_bv.byteStride > 0 ? txc_bv.byteStride : sizeof(math::Vec2<float>);
        bi = 0;
        for (auto& vertex : vertices) {
            vertex.uv = *reinterpret_cast<const math::Vec2<float>*>(&txc_b[bi]);
            bi += txc_bi_inc;
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
        bi = 0;
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
        gx_mat.get_alpha_cutoff_occlusion_strength_radiance_lod_coefficient_reserved().x = static_cast<float>(mat.alphaCutoff);
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
        gx_mat.get_alpha_cutoff_occlusion_strength_radiance_lod_coefficient_reserved().y = static_cast<float>(mat.occlusionTexture.strength);
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

    for (const tinygltf::Scene& scn : data.scenes) {
        GX_LOG_D("Loading scene: " << scn.name);
        const auto scene_builder = e.get_scene_manager()->build(scn.name);
        result.push_back(scene_builder->get_entity_builder()->get_builder().get_id());
        auto scene_end_callback = gearoenix::core::sync::EndCallerIgnored([scene_builder, end_callback = end_callback] {});
        for (const int scene_node_index : scn.nodes) {
            const tinygltf::Node& scene_node = data.nodes[scene_node_index];
            GX_LOG_D("Loading scene_node: " << scene_node.name);
            GX_ASSERT(scene_node.children.empty()); // TODO: support for node's children
            const std::vector<double>& rotation = scene_node.rotation; // length must be 0 or 4
            const std::vector<double>& scale = scene_node.scale; // length must be 0 or 3
            const std::vector<double>& translation = scene_node.translation; // length must be 0 or 3
            // const std::vector<double>& matrix = scene_node.matrix; // length must be 0 or 16
            // const std::vector<double>& weights = scene_node.weights;
            if (scene_node.camera != -1) {
                const tinygltf::Camera& cmr = data.cameras[scene_node.camera];
                GX_LOG_D("Loading camera: " << cmr.name);
                auto camera_builder = e.get_camera_manager()->build(cmr.name);
                auto& transform = camera_builder->get_transformation();
                if (scale.size() == 3)
                    transform.local_scale({ scale[0], scale[1], scale[2] });
                if (rotation.size() == 4)
                    transform.local_rotate_quaternion(rotation[0], rotation[1], rotation[2], rotation[3]);
                if (translation.size() == 3)
                    transform.set_location(translation[0], translation[1], translation[2]);
                scene_builder->add(std::move(camera_builder));
            } else if (scene_node.mesh != -1) {
                material::Pbr gx_mat(e, scene_end_callback);
                auto gx_mesh = create_mesh(scene_node.mesh, gx_mat, scene_end_callback);
                auto model_builder = e.get_model_manager()->build(
                    std::string(scene_node.name),
                    std::shared_ptr<gearoenix::render::mesh::Mesh>(gx_mesh),
                    gearoenix::core::sync::EndCallerIgnored(scene_end_callback),
                    true);
                model_builder->set_material(gx_mat);
                auto& transform = model_builder->get_transformation();
                if (scale.size() == 3)
                    transform.local_scale({ scale[0], scale[1], scale[2] });
                if (rotation.size() == 4)
                    transform.local_rotate_quaternion(rotation[0], rotation[1], rotation[2], rotation[3]);
                if (translation.size() == 3)
                    transform.set_location(translation[0], translation[1], translation[2]);
                scene_builder->add(std::move(model_builder));
            } else
                GX_UNIMPLEMENTED;
        }
    }
    return result;
}
