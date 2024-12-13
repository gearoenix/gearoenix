#include "gx-rnd-gltf-loader.hpp"
#include "../core/ecs/gx-cr-ecs-comp-allocator.hpp"
#include "../core/ecs/gx-cr-ecs-world.hpp"
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
#include "light/gx-rnd-lt-directional.hpp"
#include "light/gx-rnd-lt-light.hpp"
#include "light/gx-rnd-lt-manager.hpp"
#include "material/gx-rnd-mat-manager.hpp"
#include "material/gx-rnd-mat-pbr.hpp"
#include "mesh/gx-rnd-msh-manager.hpp"
#include "mesh/gx-rnd-msh-mesh.hpp"
#include "model/gx-rnd-mdl-builder.hpp"
#include "model/gx-rnd-mdl-manager.hpp"
#include "scene/gx-rnd-scn-builder.hpp"
#include "scene/gx-rnd-scn-manager.hpp"
#include "scene/gx-rnd-scn-scene.hpp"
#include "texture/gx-rnd-txt-manager.hpp"
#include "texture/gx-rnd-txt-texture-2d.hpp"

#include <boost/container/flat_map.hpp>

#include <unordered_set>

#define TINYGLTF_NOEXCEPTION
#define TINYGLTF_IMPLEMENTATION
#include <tinygltf/tiny_gltf.h>

namespace gearoenix::render::gltf {
constexpr auto LIGHT_EXT_NAME = "KHR_lights_punctual";

struct MeshData final {
    std::vector<std::shared_ptr<mesh::Mesh>> meshes;
    std::vector<int> bone_index_map;
    std::optional<physics::animation::BoneInfo> bones_info;
};

struct DataLoader final {
    engine::Engine& e;
    tinygltf::TinyGLTF context;
    tinygltf::Model data;

    std::vector<std::shared_ptr<texture::Texture2D>> gx_texture_2ds;
#if GX_DEBUG_MODE
    std::unordered_set<std::string> gx_textures_names;
#endif
    std::vector<MeshData> gx_meshes;
    std::vector<std::shared_ptr<material::Pbr>> gx_materials;
    boost::container::flat_map<int /*bone-node-index*/, physics::animation::BoneChannelBuilder> gx_bones_channels;

    std::weak_ptr<DataLoader> weak_self;

    [[nodiscard]] static std::shared_ptr<DataLoader> construct(engine::Engine& e)
    {
        auto result = std::make_shared<DataLoader>(e);
        result->weak_self = result;
        return result;
    }

    /// It can not be used everywhere, it has its own context. It must be used in bone tracing code.
    [[nodiscard]] static bool is_gltf_node_a_bone(const tinygltf::Node& n)
    {
        return n.skin == -1 && n.mesh == -1 && n.camera == -1 && n.extensions.empty();
    }

    // <editor-fold desc="Texture Section">

    void load_texture_2ds(const core::job::EndCaller<>& end_callback)
    {
        GX_ASSERT_D(gx_texture_2ds.empty());
        gx_texture_2ds.resize(data.textures.size());
        for (std::uint64_t index = 0; index < data.textures.size(); ++index) {
            load_texture_2d(index, core::job::EndCaller(end_callback));
        }
    }

    void load_texture_2d(
        const std::uint64_t index,
        core::job::EndCaller<>&& end_callback)
    {
        const tinygltf::Texture& txt = data.textures[index];
        GX_LOG_D("Loading texture: " << txt.name);
        const tinygltf::Image& img = data.images[txt.source];
        GX_LOG_D("Loading image: " << img.name);
#if GX_DEBUG_MODE
        {
            const auto sz_before = gx_textures_names.size();
            gx_textures_names.insert(img.name);
            const auto sz_after = gx_textures_names.size();
            GX_ASSERT_D(sz_before < sz_after);
        }
#endif
        GX_ASSERT(!img.as_is); // Only image through bufferView is supported.
        GX_ASSERT(img.bufferView >= 0); // Only image through bufferView is supported.
        GX_ASSERT(img.mimeType.ends_with("jpeg") || img.mimeType.ends_with("png")); // Only these formats are supported.
        const tinygltf::BufferView& img_bv = data.bufferViews[img.bufferView];
        const tinygltf::Buffer& img_b = data.buffers[img_bv.buffer];
        const void* const img_ptr = &img_b.data[img_bv.byteOffset];
        const auto img_sz = static_cast<std::uint32_t>(img_bv.byteLength);
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
        const auto txt_info = texture::TextureInfo()
                                  .set_format(texture::TextureFormat::Unknown)
                                  .set_sampler_info(texture::SamplerInfo()
                                          .set_min_filter(convert_filter(smp.minFilter))
                                          .set_mag_filter(convert_filter(smp.magFilter))
                                          .set_wrap_s(convert_wrap(smp.wrapS))
                                          .set_wrap_t(convert_wrap(smp.wrapT)))
                                  .set_width(0)
                                  .set_height(0)
                                  .set_depth(0)
                                  .set_type(texture::Type::Unknown)
                                  .set_has_mipmap(needs_mipmap);
        e.get_texture_manager()->create_2d_from_formatted(
            std::string(img.name), img_ptr, img_sz, txt_info,
            core::job::EndCallerShared<texture::Texture2D>(
                [index, s = weak_self.lock(), end = std::move(end_callback)](std::shared_ptr<texture::Texture2D>&& t) {
                    s->gx_texture_2ds[index] = std::move(t);
                    (void)end;
                }));
    }

    const std::shared_ptr<texture::Texture2D>& get_texture2d(
        const int index,
        const std::shared_ptr<texture::Texture2D>& default_value)
    {
        if (index < 0) {
            return default_value;
        }
        return gx_texture_2ds[index];
    }

    // </editor-fold>

    // <editor-fold desc="Mesh Section">

    [[nodiscard]] static bool is_mesh(const tinygltf::Node& node)
    {
        if (node.mesh != -1) {
            GX_ASSERT(node.children.empty());
            GX_ASSERT(node.camera == -1);
            GX_ASSERT(node.emitter == -1);
            GX_ASSERT(node.light == -1);
            return true;
        }
        return false;
    }

    [[nodiscard]] static bool is_skin(const tinygltf::Node& node)
    {
        if (node.mesh != -1 && node.skin != -1) {
            GX_ASSERT(node.children.empty());
            GX_ASSERT(node.camera == -1);
            GX_ASSERT(node.emitter == -1);
            GX_ASSERT(node.light == -1);
            return true;
        }
        return false;
    }

    void load_meshes(const core::job::EndCaller<>& end_callback)
    {
        GX_ASSERT_D(gx_meshes.empty());
        gx_meshes.resize(data.meshes.size());
        std::uint64_t index = 0;
        for (const tinygltf::Node& n : data.nodes) {
            if (!is_mesh(n)) {
                continue;
            }
            auto [bone_index_map, bones_info] = process_skin(n);
            load_mesh(n.mesh, end_callback, bone_index_map);
            gx_meshes[index].bone_index_map = std::move(bone_index_map);
            gx_meshes[index].bones_info = std::move(bones_info);
            ++index;
        }
    }

    void load_mesh(
        const std::uint64_t mesh_index,
        const core::job::EndCaller<>& end_callback,
        const std::vector<int>& bone_index_map)
    {
        const tinygltf::Mesh& msh = data.meshes[mesh_index];
        GX_LOG_D("Loading mesh: " << msh.name);
        gx_meshes[mesh_index].meshes.resize(msh.primitives.size());
        for (std::uint64_t primitive_index = 0; primitive_index < msh.primitives.size(); ++primitive_index) {
            load_mesh(mesh_index, primitive_index, end_callback, bone_index_map);
        }
    }

    void load_mesh(
        const std::uint64_t mesh_index,
        const std::uint64_t primitive_index,
        const core::job::EndCaller<>& end_callback,
        const std::vector<int>& bone_index_map) const
    {
        const tinygltf::Mesh& msh = data.meshes[mesh_index];
        const tinygltf::Primitive& primitive = msh.primitives[primitive_index];
        GX_ASSERT(TINYGLTF_MODE_TRIANGLES == primitive.mode);
        const auto& acs = data.accessors;
        const auto& bvs = data.bufferViews;
        const auto& bfs = data.buffers;
        auto pos_ai = static_cast<std::uint64_t>(-1);
        auto nrm_ai = static_cast<std::uint64_t>(-1);
        auto tng_ai = static_cast<std::uint64_t>(-1);
        auto txc_ai = static_cast<std::uint64_t>(-1);
        auto bwt_ai = static_cast<std::uint64_t>(-1);
        auto bin_ai = static_cast<std::uint64_t>(-1);
        const auto& attrs = primitive.attributes;
        const bool is_animated = !bone_index_map.empty();
        for (const auto& att : attrs) {
            if ("POSITION" == att.first) {
                pos_ai = static_cast<std::uint64_t>(att.second);
            } else if ("NORMAL" == att.first) {
                nrm_ai = static_cast<std::uint64_t>(att.second);
            } else if ("TANGENT" == att.first) {
                tng_ai = static_cast<std::uint64_t>(att.second);
            } else if ("TEXCOORD_0" == att.first) {
                txc_ai = static_cast<std::uint64_t>(att.second);
            } else if (is_animated && "WEIGHTS_0" == att.first) {
                bwt_ai = static_cast<std::uint64_t>(att.second);
            } else if (is_animated && "JOINTS_0" == att.first) {
                bin_ai = static_cast<std::uint64_t>(att.second);
            } else {
                GX_UNEXPECTED;
            }
        }
        GX_ASSERT(pos_ai != static_cast<std::uint64_t>(-1));
        GX_ASSERT(nrm_ai != static_cast<std::uint64_t>(-1));
        const auto has_tangent = tng_ai != static_cast<std::uint64_t>(-1);
        GX_ASSERT(txc_ai != static_cast<std::uint64_t>(-1));
        GX_ASSERT(!is_animated || bwt_ai != static_cast<std::uint64_t>(-1));
        GX_ASSERT(!is_animated || bin_ai != static_cast<std::uint64_t>(-1));

        GX_ASSERT(-1 != primitive.indices);
        GX_ASSERT(-1 != primitive.material);

        const auto& pos_a = acs[pos_ai];
        const auto& nrm_a = acs[nrm_ai];
        const auto* const tng_a = has_tangent ? &acs[tng_ai] : nullptr;
        const auto& txc_a = acs[txc_ai];
        const auto& bwt_a = is_animated ? acs[bwt_ai] : acs[txc_ai];
        const auto& bin_a = is_animated ? acs[bin_ai] : acs[txc_ai];
        const auto& ids_a = acs[primitive.indices];

        GX_CHECK_EQUAL(pos_a.count, nrm_a.count);
        if (has_tangent) {
            GX_CHECK_EQUAL(pos_a.count, tng_a->count);
        }
        GX_CHECK_EQUAL(pos_a.count, txc_a.count);
        GX_ASSERT(!is_animated || pos_a.count == bwt_a.count);
        GX_ASSERT(!is_animated || pos_a.count == bin_a.count);

        GX_CHECK_EQUAL(pos_a.type, TINYGLTF_TYPE_VEC3);
        GX_CHECK_EQUAL(nrm_a.type, TINYGLTF_TYPE_VEC3);
        if (has_tangent) {
            GX_CHECK_EQUAL(tng_a->type, TINYGLTF_TYPE_VEC4);
        }
        GX_CHECK_EQUAL(txc_a.type, TINYGLTF_TYPE_VEC2);
        GX_ASSERT(!is_animated || bwt_a.type == TINYGLTF_TYPE_VEC4);
        GX_ASSERT(!is_animated || bin_a.type == TINYGLTF_TYPE_VEC4);
        GX_CHECK_EQUAL(ids_a.type, TINYGLTF_TYPE_SCALAR);

        GX_CHECK_EQUAL(pos_a.componentType, TINYGLTF_COMPONENT_TYPE_FLOAT);
        GX_CHECK_EQUAL(nrm_a.componentType, TINYGLTF_COMPONENT_TYPE_FLOAT);
        if (has_tangent) {
            GX_CHECK_EQUAL(tng_a->componentType, TINYGLTF_COMPONENT_TYPE_FLOAT);
        }
        GX_CHECK_EQUAL(txc_a.componentType, TINYGLTF_COMPONENT_TYPE_FLOAT);
        GX_ASSERT(!is_animated || bwt_a.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

        const auto& pos_max = pos_a.maxValues;
        const auto& pos_min = pos_a.minValues;
        GX_CHECK_EQUAL(pos_max.size(), 3);
        GX_CHECK_EQUAL(pos_min.size(), 3);

        const math::Aabb3<double> box(
            math::Vec3<double>(pos_max[0], pos_max[1], pos_max[2]),
            math::Vec3<double>(pos_min[0], pos_min[1], pos_min[2]));

        std::vector<PbrVertex> vertices(pos_a.count);
        std::vector<PbrVertexAnimated> animated_vertices(is_animated ? pos_a.count : 0);
        std::vector<std::uint32_t> indices(ids_a.count);

        const auto& pos_bv = bvs[pos_a.bufferView];
        const auto& nrm_bv = bvs[nrm_a.bufferView];
        const auto* const tng_bv = has_tangent ? &bvs[tng_a->bufferView] : nullptr;
        const auto& txc_bv = bvs[txc_a.bufferView];
        const auto& bwt_bv = is_animated ? bvs[bwt_a.bufferView] : bvs[txc_a.bufferView];
        const auto& bin_bv = is_animated ? bvs[bin_a.bufferView] : bvs[txc_a.bufferView];
        const auto& ids_bv = bvs[ids_a.bufferView];

        {
            const auto* const pos_b = &bfs[pos_bv.buffer].data[pos_bv.byteOffset];
            const std::uint64_t pos_bi_inc = pos_bv.byteStride > 0 ? pos_bv.byteStride : sizeof(math::Vec3<float>);
            std::uint64_t bi = 0;
            for (auto& vertex : vertices) {
                vertex.position = *reinterpret_cast<const math::Vec3<float>*>(&pos_b[bi]);
                bi += pos_bi_inc;
            }
        }

        {
            const auto* const nrm_b = &bfs[nrm_bv.buffer].data[nrm_bv.byteOffset];
            const std::uint64_t nrm_bi_inc = nrm_bv.byteStride > 0 ? nrm_bv.byteStride : sizeof(math::Vec3<float>);
            std::uint64_t bi = 0;
            for (auto& vertex : vertices) {
                vertex.normal = *reinterpret_cast<const math::Vec3<float>*>(&nrm_b[bi]);
                bi += nrm_bi_inc;
            }
        }

        if (has_tangent) {
            const auto* const tng_b = &bfs[tng_bv->buffer].data[tng_bv->byteOffset];
            GX_ASSERT_D(tng_bv->byteStride <= 0 || tng_bv->byteStride >= sizeof(math::Vec4<float>));
            const std::uint64_t tng_bi_inc = tng_bv->byteStride > 0 ? tng_bv->byteStride : sizeof(math::Vec4<float>);
            std::uint64_t bi = 0;
            for (auto& vertex : vertices) {
                vertex.tangent = *reinterpret_cast<const math::Vec4<float>*>(&tng_b[bi]);
                bi += tng_bi_inc;
            }
        }

        {
            const auto* const txc_b = &bfs[txc_bv.buffer].data[txc_bv.byteOffset];
            const std::uint64_t txc_bi_inc = txc_bv.byteStride > 0 ? txc_bv.byteStride : sizeof(math::Vec2<float>);
            std::uint64_t bi = 0;
            for (auto& vertex : vertices) {
                vertex.uv = *reinterpret_cast<const math::Vec2<float>*>(&txc_b[bi]);
                vertex.uv.y = 1.0f - vertex.uv.y;
                bi += txc_bi_inc;
            }
        }

        if (is_animated) {
            {
                const auto* const bwt_b = &bfs[bwt_bv.buffer].data[bwt_bv.byteOffset];
                const std::uint64_t bwt_bi_inc = bwt_bv.byteStride > 0 ? bwt_bv.byteStride : sizeof(math::Vec4<float>);
                std::uint64_t bi = 0;
                for (auto& vertex : animated_vertices) {
                    vertex.bone_weights = *reinterpret_cast<const math::Vec4<float>*>(&bwt_b[bi]);
                    bi += bwt_bi_inc;
                }
            }

            {
                const auto* const bin_b = &bfs[bin_bv.buffer].data[bin_bv.byteOffset];
                const auto bin_bi_inc = [&]() -> std::uint64_t {
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
                std::uint64_t bi = 0;
                const auto convert_indices = [&]<typename T>() {
                    auto is = math::Vec4<int>(*reinterpret_cast<const math::Vec4<T>*>(&bin_b[bi]));
                    is.x = bone_index_map[is.x];
                    is.y = bone_index_map[is.y];
                    is.z = bone_index_map[is.z];
                    is.w = bone_index_map[is.w];
                    return math::Vec4<float>(is) + math::Numeric::epsilon<float>;
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
        const std::uint64_t ids_bi_inc = [&]() {
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
        std::uint64_t bi = 0;
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
                i = *reinterpret_cast<const std::uint8_t*>(&ids_b[bi]);
                bi += ids_bi_inc;
            }
            break;
        default:
            GX_UNEXPECTED;
        }

        if (!has_tangent) {
            calculate_tangents(vertices, indices);
        }

        for (std::uint64_t vi = 0; vi < animated_vertices.size(); ++vi) {
            animated_vertices[vi].base = vertices[vi];
        }

        core::job::EndCallerShared<mesh::Mesh> mesh_end_callback([mesh_index, primitive_index, s = weak_self.lock(), end_callback](std::shared_ptr<mesh::Mesh>&& m) {
            s->gx_meshes[mesh_index].meshes[primitive_index] = std::move(m);
            (void)end_callback;
        });

        auto name = msh.name + "-primitive:" + std::to_string(primitive_index);
        if (is_animated) {
            e.get_mesh_manager()->build(
                std::move(name),
                std::move(animated_vertices),
                std::move(indices),
                box,
                std::shared_ptr(gx_materials[primitive.material]),
                std::move(mesh_end_callback));
        } else {
            e.get_mesh_manager()->build(
                std::move(name),
                std::move(vertices),
                std::move(indices),
                box,
                std::shared_ptr(gx_materials[primitive.material]),
                std::move(mesh_end_callback));
        }
    }

    // </editor-fold>

    [[nodiscard]] static bool is_light(const tinygltf::Node& node)
    {
        if (node.light != -1 || (node.extensions.size() == 1 && node.extensions.begin()->first == LIGHT_EXT_NAME)) {
            GX_ASSERT_D(node.children.empty());
            GX_ASSERT_D(node.camera == -1);
            GX_ASSERT_D(node.emitter == -1);
            GX_ASSERT_D(node.mesh == -1);
            GX_ASSERT_D(node.skin == -1);
            return true;
        }
        return false;
    }

    [[nodiscard]] static bool is_camera(const tinygltf::Node& node)
    {
        if (node.camera != -1) {
            GX_ASSERT_D(node.children.empty());
            GX_ASSERT_D(node.emitter == -1);
            GX_ASSERT_D(node.light == -1);
            GX_ASSERT_D(node.mesh == -1);
            GX_ASSERT_D(node.skin == -1);
            return true;
        }
        return false;
    }

    void apply_transform(const std::uint64_t node_index, physics::Transformation& transform) const
    {
        apply_transform(data.nodes[node_index], transform);
    }

    static void apply_transform(const tinygltf::Node& node, physics::Transformation& transform)
    {
        const std::vector<double>& rotation = node.rotation;
        GX_ASSERT_D(rotation.empty() || rotation.size() == 4);
        const std::vector<double>& scale = node.scale;
        GX_ASSERT_D(scale.empty() || scale.size() == 3);
        const std::vector<double>& translation = node.translation;
        GX_ASSERT_D(translation.empty() || translation.size() == 3);
        if (scale.size() == 3) {
            transform.local_inner_scale({ scale[0], scale[1], scale[2] });
        }
        if (rotation.size() == 4) {
            transform.local_inner_rotate(math::Quat<double>(rotation[0], rotation[1], rotation[2], rotation[3]));
        }
        if (translation.size() == 3) {
            transform.set_local_position(math::Vec3<double>(translation[0], translation[1], translation[2]));
        }
    }

    physics::animation::BoneInfo load_bone_info(
        const int bone_node_index,
        const std::string& parent_name,
        const std::map<std::string, int>& name_to_joint_index,
        const math::Mat4x4<float>* inv_bind_mats)
    {
        const tinygltf::Node& bone_node = data.nodes[bone_node_index];
        physics::animation::BoneInfo bone_info(std::string(bone_node.name));
        bone_info.parent_name = parent_name;
        {
            const auto search = name_to_joint_index.find(bone_node.name);
            GX_ASSERT_D(name_to_joint_index.end() != search);
            bone_info.inverse_bind = inv_bind_mats[search->second];
        }
        apply_transform(bone_node_index, *bone_info.transform);
        bone_info.children.reserve(bone_node.children.size());
        for (auto child : bone_node.children) {
            bone_info.children.push_back(load_bone_info(child, bone_node.name, name_to_joint_index, inv_bind_mats));
        }
        return bone_info;
    }

    static void set_bone_map(
        const physics::animation::BoneInfo& bone_info,
        const std::map<std::string, int>& name_to_joint_index,
        std::vector<int>& bone_index_map,
        int& index)
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

    std::pair<std::vector<int>, std::optional<physics::animation::BoneInfo>> process_skin(const tinygltf::Node& node)
    {
        if (node.skin == -1) {
            return std::make_pair(std::vector<int>(), std::nullopt);
        }
        const auto& skin = data.skins[node.skin];
        std::vector<int> bone_index_map(skin.joints.size());
        GX_ASSERT_D(-1 != skin.inverseBindMatrices);
        const auto& inv_bind_mat_acc = data.accessors[skin.inverseBindMatrices];
        GX_ASSERT_D(inv_bind_mat_acc.count == skin.joints.size());
        GX_ASSERT_D(inv_bind_mat_acc.type == TINYGLTF_TYPE_MAT4);
        GX_ASSERT_D(inv_bind_mat_acc.bufferView != -1);
        const auto& inv_bind_mat_bv = data.bufferViews[inv_bind_mat_acc.bufferView];
        GX_ASSERT_D(inv_bind_mat_bv.byteLength == inv_bind_mat_acc.count * sizeof(math::Mat4x4<float>));
        GX_ASSERT_D(inv_bind_mat_bv.byteStride == 0);
        const auto inv_bind_mats = reinterpret_cast<const math::Mat4x4<float>*>(
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
        auto bones_info = load_bone_info(*root_bones_nodes.begin(), "", name_to_joint_index, inv_bind_mats);
        joint_index = 0;
        {
            const auto search = name_to_joint_index.find(bones_info.name);
            GX_ASSERT_D(name_to_joint_index.end() != search);
            bone_index_map[search->second] = joint_index;
        }
        set_bone_map(bones_info, name_to_joint_index, bone_index_map, joint_index);
        return { std::move(bone_index_map), std::move(bones_info) };
    }

    [[nodiscard]] bool process_node_camera(
        const std::uint64_t node_index,
        physics::Transformation* const parent_transform,
        const core::job::EndCaller<>& gpu_end_callback,
        const core::job::EndCaller<>& entity_end_callback,
        const std::shared_ptr<scene::Builder>& scene_builder) const
    {
        const tinygltf::Node& node = data.nodes[node_index];
        if (!is_camera(node)) {
            return false;
        }
        const tinygltf::Camera& cmr = data.cameras[node.camera];
        GX_LOG_D("Loading camera: " << cmr.name);
        e.get_camera_manager()->build(
            cmr.name,
            parent_transform,
            core::job::EndCallerShared<camera::Builder>([node_index, scene_builder, gpu_end_callback, s = weak_self.lock()](std::shared_ptr<camera::Builder>&& camera_builder) {
                const tinygltf::Node& node = s->data.nodes[node_index];
                const tinygltf::Camera& cmr = s->data.cameras[node.camera];
                auto& rnd_cmr = camera_builder->get_camera();
                if ("perspective" == cmr.type) {
                    GX_ASSERT(cmr.perspective.znear < cmr.perspective.zfar);
                    GX_ASSERT(cmr.perspective.znear > 0.0);
                    rnd_cmr.set_projection_data(camera::ProjectionData::construct_perspective(static_cast<float>(cmr.perspective.yfov)));
                    rnd_cmr.set_far(static_cast<float>(cmr.perspective.zfar));
                    rnd_cmr.set_near(static_cast<float>(cmr.perspective.znear));
                } else {
                    GX_ASSERT(cmr.orthographic.xmag == cmr.orthographic.ymag);
                    GX_ASSERT(cmr.orthographic.xmag > 0.0);
                    rnd_cmr.set_projection_data(camera::ProjectionData::construct_orthographic(static_cast<float>(cmr.orthographic.xmag)));
                    rnd_cmr.set_far(static_cast<float>(cmr.orthographic.zfar));
                    rnd_cmr.set_near(static_cast<float>(cmr.orthographic.znear));
                }
                auto& transform = camera_builder->get_transformation();
                apply_transform(node, transform);
                scene_builder->add(std::move(camera_builder));
                (void)gpu_end_callback;
            }),
            core::job::EndCaller(entity_end_callback));
        return true;
    }

    [[nodiscard]] bool process_node_mesh(
        const tinygltf::Node& node,
        physics::Transformation* const parent_transform,
        const core::job::EndCaller<>& entity_end_callback,
        scene::Builder& scene_builder)
    {
        if (!is_mesh(node)) {
            return false;
        }
        auto model_builder = e.get_model_manager()->build(
            std::string(node.name),
            parent_transform,
            std::vector(gx_meshes[node.mesh].meshes),
            core::job::EndCaller(entity_end_callback),
            true);
        if (gx_meshes[node.mesh].bone_index_map.size() > 1) {
            e.get_physics_engine()->get_animation_manager()->create_armature(
                model_builder->get_entity_builder()->get_builder(),
                gx_meshes[node.mesh].bones_info.value());
            const auto& skin = data.skins[node.skin];
            physics::animation::ArmatureAnimationInfo armature_animation_info;
            for (const auto bone_node_index : skin.joints) {
                auto bone_channel_search = gx_bones_channels.find(bone_node_index);
                GX_ASSERT(gx_bones_channels.end() != bone_channel_search);
                armature_animation_info.channels.push_back(bone_channel_search->second);
            }
            armature_animation_info.optimise();
            e.get_physics_engine()->get_animation_manager()->create_animation_player(
                model_builder->get_entity_builder()->get_builder(),
                armature_animation_info);
        }
        apply_transform(node, model_builder->get_transformation());
        scene_builder.add(std::move(model_builder));
        return true;
    }

    [[nodiscard]] bool process_node_light(
        const std::uint64_t node_index,
        physics::Transformation* const parent_transform,
        const core::job::EndCaller<>& light_end_callback,
        const core::job::EndCaller<>& entity_end_callback,
        const std::shared_ptr<scene::Builder>& scene_builder)
    {
        const tinygltf::Node& node = data.nodes[node_index];
        if (!is_light(node)) {
            return false;
        }
        const auto& light_ext = node.extensions.begin()->second;
        GX_ASSERT_D(light_ext.IsObject());
        const auto light_index = light_ext.Get("light").GetNumberAsInt();
        auto light_ext_search = data.extensions.find(LIGHT_EXT_NAME);
        GX_ASSERT_D(data.extensions.end() != light_ext_search);
        const auto& lights = light_ext_search->second.Get("lights");
        GX_ASSERT_D(lights.IsArray());
        const auto& light_info = lights.Get(light_index);
        GX_ASSERT_D(light_info.IsObject());
        const auto& json_light_colour = light_info.Get("color");
        GX_ASSERT_D(json_light_colour.IsArray());
        GX_ASSERT_D(3 == json_light_colour.ArrayLen());
        const auto colour_x = static_cast<float>(json_light_colour.Get(0).GetNumberAsDouble());
        const auto colour_y = static_cast<float>(json_light_colour.Get(1).GetNumberAsDouble());
        const auto colour_z = static_cast<float>(json_light_colour.Get(2).GetNumberAsDouble());
        GX_ASSERT_D(0.0f <= colour_x);
        GX_ASSERT_D(1.0f >= colour_x);
        GX_ASSERT_D(0.0f <= colour_y);
        GX_ASSERT_D(1.0f >= colour_y);
        GX_ASSERT_D(0.0f <= colour_z);
        GX_ASSERT_D(1.0f >= colour_z);
        const auto intensity = static_cast<float>(light_info.Get("intensity").GetNumberAsDouble() / 683.0);
        const auto colour = math::Vec3(colour_x, colour_y, colour_z) * intensity;
        const auto light_type = light_info.Get("type").Get<std::string>();
        GX_ASSERT_D(light_type == "directional");
        e.get_light_manager()->build_shadow_caster_directional(
            node.name, parent_transform, 1024, 20.0f, 1.0f, 35.0f,
            core::job::EndCallerShared<light::Builder>([sb = scene_builder, node_index, s = weak_self.lock(), le = light_end_callback, colour](std::shared_ptr<light::Builder>&& lb) {
                lb->get_light().colour = colour;
                s->apply_transform(node_index, *lb->get_shadow_caster_directional()->get_shadow_transform());
                sb->add(std::move(lb));
                (void)le;
            }),
            core::job::EndCaller(entity_end_callback));
        return true;
    }

    [[nodiscard]] bool is_armature(const tinygltf::Node& node) const
    {
        if (node.camera != -1) {
            return false;
        }
        if (node.emitter != -1) {
            return false;
        }
        if (node.light != -1) {
            return false;
        }
        if (node.mesh != -1) {
            return false;
        }
        if (node.skin != -1) {
            return false;
        }
        if (node.children.size() != 2) {
            return false;
        }
        const auto first_skin = is_skin(data.nodes[node.children[0]]);
        const auto second_skin = is_skin(data.nodes[node.children[1]]);
        if (!first_skin && !second_skin) {
            return false;
        }
        GX_ASSERT_D(first_skin != second_skin);
        return true;
    }

    [[nodiscard]] bool is_empty(const tinygltf::Node& node) const
    {
        if (node.camera != -1) {
            return false;
        }
        if (node.emitter != -1) {
            return false;
        }
        if (node.light != -1) {
            return false;
        }
        if (node.mesh != -1) {
            return false;
        }
        if (node.skin != -1) {
            return false;
        }
        if (is_armature(node)) {
            return false;
        }
        GX_COMPLAIN_D(node.children.size() != 1 || !has_transformation(node),
            "What is expected here is an empty node for parenting several other entities and "
            "parenting only one child is a waste of transform operation, obviously child can replace this one.");
        return true;
    }

    [[nodiscard]] bool process_node_armature(
        const int node_index,
        physics::Transformation* const parent_transform,
        const core::job::EndCaller<>& gpu_end_callback,
        const core::job::EndCaller<>& entity_end_callback,
        const std::shared_ptr<scene::Builder>& scene_builder)
    {
        const auto& node = data.nodes[node_index];
        if (!is_armature(node)) {
            return false;
        }
        auto* const transform = create_empty_entity_transform(parent_transform, node, entity_end_callback, scene_builder);
        if (const auto mesh_node_index = node.children[0]; is_skin(data.nodes[mesh_node_index])) {
            process_node(mesh_node_index, transform, gpu_end_callback, entity_end_callback, scene_builder);
            return true;
        }
        if (const auto mesh_node_index = node.children[1]; is_skin(data.nodes[mesh_node_index])) {
            process_node(mesh_node_index, transform, gpu_end_callback, entity_end_callback, scene_builder);
            return true;
        }
        GX_LOG_F("The correct node for skin and mesh not found. Node: " << node.name);
    }

    [[nodiscard]] static bool has_transformation(const tinygltf::Node& node)
    {
        return node.rotation.empty() && node.scale.empty() && node.translation.empty();
    }

    [[nodiscard]] physics::Transformation* create_empty_entity_transform(
        physics::Transformation* const parent_transform,
        const tinygltf::Node& node,
        const core::job::EndCaller<>& entity_end_callback,
        const std::shared_ptr<scene::Builder>& scene_builder) const
    {
        if (has_transformation(node)) {
            return parent_transform;
        } else {
            GX_ASSERT_D(!node.name.empty());
            const auto entity_builder = std::make_shared<core::ecs::EntitySharedBuilder>(
                std::string(node.name),
                core::job::EndCaller(entity_end_callback));
            const auto transform = core::ecs::construct_component<physics::Transformation>(
                node.name + "-transformation", parent_transform, entity_builder->get_id(), &e);
            entity_builder->get_builder().add_component(transform);
            scene_builder->get_scene().add_empty(entity_builder->get_id());
            apply_transform(node, *transform);
            return transform.get();
        }
    }

    [[nodiscard]] bool process_node_empty(
        const int node_index,
        physics::Transformation* const parent_transform,
        const core::job::EndCaller<>& gpu_end_callback,
        const core::job::EndCaller<>& entity_end_callback,
        const std::shared_ptr<scene::Builder>& scene_builder)
    {
        const auto& node = data.nodes[node_index];
        if (!is_empty(node)) {
            return false;
        }
        const auto transform = create_empty_entity_transform(parent_transform, node, entity_end_callback, scene_builder);
        for (const auto child : node.children) {
            process_node(child, transform, gpu_end_callback, entity_end_callback, scene_builder);
        }
        return true;
    }

    void process_node(
        const int node_index,
        physics::Transformation* const parent_transform,
        const core::job::EndCaller<>& gpu_end_callback,
        const core::job::EndCaller<>& entity_end_callback,
        const std::shared_ptr<scene::Builder>& scene_builder)
    {
        const auto& node = data.nodes[node_index];
        GX_LOG_D("Loading node: " << node.name);
        if (process_node_camera(node_index, parent_transform, gpu_end_callback, entity_end_callback, scene_builder)) {
            return;
        }
        if (process_node_mesh(node, parent_transform, entity_end_callback, *scene_builder)) {
            return;
        }
        if (process_node_light(node_index, parent_transform, gpu_end_callback, entity_end_callback, scene_builder)) {
            return;
        }
        if (process_node_armature(node_index, parent_transform, gpu_end_callback, entity_end_callback, scene_builder)) {
            return;
        }
        if (process_node_empty(node_index, parent_transform, gpu_end_callback, entity_end_callback, scene_builder)) {
            return;
        }
        GX_LOG_F("Unexpected node in the scene nodes. Node: " << node.name);
    }

    template <template <typename> typename Value>
    static void read_output(
        std::vector<std::pair<double, physics::animation::Keyframe<Value<double>>>>& keyframes,
        const tinygltf::Accessor& input,
        const tinygltf::Accessor& output,
        const tinygltf::BufferView& output_bv,
        const physics::animation::Interpolation interpolation,
        const std::uint64_t output_b_ptr,
        const std::vector<float>& times)
    {
        keyframes.reserve(input.count);
        const auto output_bytes_count = output.count * sizeof(Value<float>);
        GX_COMPLAIN_D(output_bytes_count == output_bv.byteLength, "Inefficient gltf exporter for keyframes.");
        GX_ASSERT_D(output_bytes_count <= output_bv.byteLength);
        switch (interpolation) {
        case physics::animation::Interpolation::Gltf2CubicSPLine: {
            GX_ASSERT_D(input.count * sizeof(Value<float>) * 3 == output_bytes_count);
            for (std::uint64_t data_i = 0, curr_output_ptr = output_b_ptr;
                data_i < input.count; ++data_i, curr_output_ptr += sizeof(Value<float>)) {
                const std::uint64_t in_ptr = curr_output_ptr;
                const auto in = Value<double>(*reinterpret_cast<const Value<float>*>(in_ptr));
                curr_output_ptr += sizeof(Value<float>);
                const std::uint64_t key_ptr = curr_output_ptr;
                const auto key = Value<double>(*reinterpret_cast<const Value<float>*>(key_ptr));
                curr_output_ptr += sizeof(Value<float>);
                const std::uint64_t out_ptr = curr_output_ptr;
                const auto out = Value<double>(*reinterpret_cast<const Value<float>*>(out_ptr));
                keyframes.emplace_back(
                    times[data_i],
                    physics::animation::Keyframe<Value<double>> {
                        physics::animation::KeyframeGltf2Bezier<Value<double>> {
                            .key = key,
                            .in = in,
                            .out = out,
                        } });
            }
            break;
        }
        case physics::animation::Interpolation::Linear: {
            GX_ASSERT_D(input.count * sizeof(Value<float>) == output_bytes_count);
            for (std::uint64_t data_i = 0, curr_output_ptr = output_b_ptr;
                data_i < input.count; ++data_i, curr_output_ptr += sizeof(Value<float>)) {
                keyframes.emplace_back(
                    times[data_i],
                    physics::animation::KeyframeLinear<Value<double>> {
                        Value<double>(*reinterpret_cast<const Value<float>*>(curr_output_ptr)) });
            }
            break;
        }
        case physics::animation::Interpolation::Step: {
            GX_ASSERT_D(input.count * sizeof(Value<float>) == output_bytes_count);
            for (std::uint64_t data_i = 0, curr_output_ptr = output_b_ptr;
                data_i < input.count; ++data_i, curr_output_ptr += sizeof(Value<float>)) {
                keyframes.emplace_back(
                    times[data_i],
                    physics::animation::KeyframeStep<Value<double>> {
                        Value<double>(*reinterpret_cast<const Value<float>*>(curr_output_ptr)) });
            }
            break;
        }
        }
    }

    [[nodiscard]] static physics::animation::Interpolation convert_interpolation(const std::string& interpolation)
    {
        if ("CUBICSPLINE" == interpolation) {
            return physics::animation::Interpolation::Gltf2CubicSPLine;
        }
        if ("LINEAR" == interpolation) {
            return physics::animation::Interpolation::Linear;
        }
        if ("STEP" == interpolation) {
            return physics::animation::Interpolation::Step;
        }
        GX_LOG_F("Unexpected interpolation " << interpolation);
    }

    void initialise_material(const std::uint64_t index)
    {
        tinygltf::Material& mat = data.materials[index];
        material::Pbr& gx_mat = *gx_materials[index];
        GX_LOG_D("Loading material: " << mat.name);
        GX_COMPLAIN_D(!mat.doubleSided, "We don't support double sided object yet! In material: " << mat.name);
        const auto& albedo_factor = mat.pbrMetallicRoughness.baseColorFactor;
        GX_ASSERT(albedo_factor.size() == 4);
        gx_mat.get_alpha_cutoff_occlusion_strength_reserved_reserved().x = static_cast<float>(mat.alphaCutoff);
        if ("OPAQUE" == mat.alphaMode) {
            gx_mat.set_transparency(material::Transparency::Opaque);
        } else if ("TRANSPARENT" == mat.alphaMode) {
            gx_mat.set_transparency(material::Transparency::Transparent);
        } else {
            GX_UNEXPECTED;
        }
        GX_ASSERT(mat.emissiveFactor.size() == 3);
        gx_mat.get_emission_roughness_factor().x = static_cast<float>(mat.emissiveFactor[0]);
        gx_mat.get_emission_roughness_factor().y = static_cast<float>(mat.emissiveFactor[1]);
        gx_mat.get_emission_roughness_factor().z = static_cast<float>(mat.emissiveFactor[2]);
        gx_mat.set_emission(std::shared_ptr(get_texture2d(mat.emissiveTexture.index, gx_mat.get_emission())));
        gx_mat.set_normal(std::shared_ptr(get_texture2d(mat.normalTexture.index, gx_mat.get_normal())));
        gx_mat.get_normal_metallic_factor().x = static_cast<float>(mat.normalTexture.scale);
        gx_mat.get_normal_metallic_factor().y = static_cast<float>(mat.normalTexture.scale);
        gx_mat.get_normal_metallic_factor().z = 1.0f; // strange but tinygltf docs says that
        gx_mat.set_occlusion(std::shared_ptr(get_texture2d(mat.occlusionTexture.index, gx_mat.get_occlusion())));
        gx_mat.get_alpha_cutoff_occlusion_strength_reserved_reserved().y = static_cast<float>(mat.occlusionTexture.strength);
        gx_mat.set_albedo(std::shared_ptr(get_texture2d(mat.pbrMetallicRoughness.baseColorTexture.index, gx_mat.get_albedo())));
        gx_mat.get_albedo_factor() = math::Vec4<float>(
            static_cast<float>(albedo_factor[0]),
            static_cast<float>(albedo_factor[1]),
            static_cast<float>(albedo_factor[2]),
            static_cast<float>(albedo_factor[3]));
        gx_mat.set_metallic_roughness(std::shared_ptr(get_texture2d(mat.pbrMetallicRoughness.metallicRoughnessTexture.index, gx_mat.get_metallic_roughness())));
        gx_mat.get_emission_roughness_factor().w = static_cast<float>(mat.pbrMetallicRoughness.roughnessFactor);
        gx_mat.get_normal_metallic_factor().w = static_cast<float>(mat.pbrMetallicRoughness.metallicFactor);
    }

    void load_material(const std::uint64_t index, core::job::EndCaller<>&& end_callback)
    {
        gx_materials.resize(data.materials.size());
        e.get_material_manager()->get_pbr(
            std::string(data.materials[index].name),
            core::job::EndCallerShared<material::Pbr>([end = std::move(end_callback), index, s = weak_self.lock()](std::shared_ptr<material::Pbr>&& m) {
                s->gx_materials[index] = std::move(m);
                s->initialise_material(index);
                (void)end;
            }));
    }

    void load_materials(const core::job::EndCaller<>& end_callback)
    {
        for (std::uint64_t material_id = 0; material_id < data.materials.size(); ++material_id) {
            load_material(material_id, core::job::EndCaller(end_callback));
        }
    }

    void load_animations()
    {
        for (const tinygltf::Animation& anm : data.animations) {
            for (const tinygltf::AnimationChannel& chn : anm.channels) {
                const auto& bone_node = data.nodes[chn.target_node];
                GX_ASSERT(is_gltf_node_a_bone(bone_node));
                physics::animation::BoneChannelBuilder& bone_channels = gx_bones_channels[chn.target_node];
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
                const auto input_byte_length = input.count * sizeof(float);
                GX_COMPLAIN_D(input_byte_length == input_bv.byteLength, "Wast of memory in gltf animation sample, probably inefficient exporter");
                GX_ASSERT_D(input_byte_length <= input_bv.byteLength);
                GX_ASSERT_D(0 == input_bv.byteStride);
                GX_ASSERT_D(0 == output_bv.byteStride);
                std::memcpy(times.data(), &data.buffers[input_bv.buffer].data[input_bv.byteOffset], input_byte_length);
                const std::vector<std::uint8_t>& output_b = data.buffers[output_bv.buffer].data;
                const auto output_b_ptr = reinterpret_cast<std::uint64_t>(&output_b[output_bv.byteOffset]);
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
    }

    void load_scenes(
        const core::job::EndCaller<std::vector<std::shared_ptr<scene::Builder>>>& scenes_end_callback,
        const core::job::EndCaller<>& entity_end_callback)
    {
        scenes_end_callback.set_return(std::vector<std::shared_ptr<scene::Builder>>(data.scenes.size()));

        load_animations();

        const core::job::EndCaller<> meshes_ready([this, s = weak_self.lock(), scenes_end_callback, entity_end_callback] {
            const core::job::EndCaller<> gpu_end_callback([s = scenes_end_callback] { (void)s; });
            for (std::uint64_t index = 0; index < data.scenes.size(); ++index) {
                const tinygltf::Scene& scn = data.scenes[index];
                GX_LOG_D("Loading scene: " << scn.name);
                auto scene_builder = e.get_scene_manager()->build(scn.name, 0.0, core::job::EndCaller(entity_end_callback));
                for (const int scene_node_index : scn.nodes) {
                    process_node(scene_node_index, nullptr, gpu_end_callback, entity_end_callback, scene_builder);
                }
                scenes_end_callback.get_return()[index] = std::move(scene_builder);
            }
            (void)s;
        });

        const core::job::EndCaller<> materials_ready([s = weak_self.lock(), end = meshes_ready] {
            s->load_meshes(end);
        });

        const core::job::EndCaller<> textures_ready([s = weak_self.lock(), end = materials_ready] {
            s->load_materials(end);
        });

        load_texture_2ds(textures_ready);
    }

    void read_gltf(const platform::stream::Path& file)
    {
        std::string err, warn;
        const auto stream = platform::stream::Stream::open(file, e.get_platform_application());
        GX_ASSERT(nullptr != stream);
        const auto file_data = stream->get_file_content();
        if (!context.LoadBinaryFromMemory(
                &data, &err, &warn, file_data.data(),
                static_cast<unsigned int>(file_data.size()))
            || !err.empty()) {
            GX_LOG_F("Error in GLTF loader: " << err);
        }
        if (!warn.empty()) {
            GX_LOG_E("Warning in GLTF loader: " << warn);
        }
    }
};

void load(
    engine::Engine& e,
    const platform::stream::Path& file,
    const core::job::EndCaller<std::vector<std::shared_ptr<scene::Builder>>>& scenes_end_callback,
    const core::job::EndCaller<>& entity_end_callback)
{
    core::job::send_job_to_pool([&, file = file, scenes_end_callback, entity_end_callback] {
        auto loader = DataLoader::construct(e);
        loader->read_gltf(file);
        loader->load_scenes(scenes_end_callback, entity_end_callback);
    });
}
}
