#include "../../core/ecs/gx-cr-ecs-entity-builder.hpp"
#include "../../physics/animation/gx-phs-anm-bone.hpp"
#include "../../physics/animation/gx-phs-anm-manager.hpp"
#include "../../physics/gx-phs-engine.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../material/gx-rnd-mat-pbr.hpp"
#include "../mesh/gx-rnd-msh-manager.hpp"
#include "../mesh/gx-rnd-msh-mesh.hpp"
#include "../model/gx-rnd-mdl-builder.hpp"
#include "../model/gx-rnd-mdl-manager.hpp"
#include "../scene/gx-rnd-scn-builder.hpp"
#include "gx-rnd-gltf-context.hpp"
#include "gx-rnd-gltf-transform.hpp"
#include <set>

namespace gearoenix::render::gltf {
struct Mesh final {
    std::vector<std::shared_ptr<mesh::Mesh>> meshes;
    std::vector<int> bone_index_map;
    std::optional<physics::animation::BoneInfo> bones_info;
};

physics::animation::BoneInfo load_bone_info(
    const int bone_node_index,
    const Context& context,
    const std::string& parent_name,
    const std::map<std::string, int>& name_to_joint_index,
    const math::Mat4x4<float>* inv_bind_mats)
{
    const auto& bone_node = context.data.nodes[bone_node_index];
    physics::animation::BoneInfo bone_info(std::string(bone_node.name));
    bone_info.parent_name = parent_name;
    {
        const auto search = name_to_joint_index.find(bone_node.name);
        GX_ASSERT_D(name_to_joint_index.end() != search);
        bone_info.inverse_bind = inv_bind_mats[search->second];
    }
    apply_transform(bone_node_index, context, *bone_info.transform);
    bone_info.children.reserve(bone_node.children.size());
    for (const auto child : bone_node.children) {
        bone_info.children.push_back(load_bone_info(child, context, bone_node.name, name_to_joint_index, inv_bind_mats));
    }
    return bone_info;
}

void set_bone_map(
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

std::pair<std::vector<int>, std::optional<physics::animation::BoneInfo>> process_skin(
    const Context& context, const int node_index)
{
    const auto& node = context.data.nodes[node_index];
    if (node.skin == -1) {
        return std::make_pair(std::vector<int>(), std::nullopt);
    }
    const auto& skin = context.data.skins[node.skin];
    std::vector<int> bone_index_map(skin.joints.size());
    GX_ASSERT_D(-1 != skin.inverseBindMatrices);
    const auto& inv_bind_mat_acc = context.data.accessors[skin.inverseBindMatrices];
    GX_ASSERT_D(inv_bind_mat_acc.count == skin.joints.size());
    GX_ASSERT_D(inv_bind_mat_acc.type == TINYGLTF_TYPE_MAT4);
    GX_ASSERT_D(inv_bind_mat_acc.bufferView != -1);
    const auto& inv_bind_mat_bv = context.data.bufferViews[inv_bind_mat_acc.bufferView];
    GX_ASSERT_D(inv_bind_mat_bv.byteLength == inv_bind_mat_acc.count * sizeof(math::Mat4x4<float>));
    GX_ASSERT_D(inv_bind_mat_bv.byteStride == 0);
    const auto inv_bind_mats = reinterpret_cast<const math::Mat4x4<float>*>(
        &context.data.buffers[inv_bind_mat_bv.buffer].data[inv_bind_mat_bv.byteOffset]);
    std::set<int> root_bones_nodes;
    std::map<std::string, int> name_to_joint_index;
    int joint_index = 0;
    for (const auto bone_node_index : skin.joints) {
        root_bones_nodes.emplace(bone_node_index);
        name_to_joint_index[context.data.nodes[bone_node_index].name] = joint_index;
        ++joint_index;
    }
    for (const auto bone_node_index : skin.joints) {
        const auto& bone_node = context.data.nodes[bone_node_index];
        for (const auto child_node_index : bone_node.children) {
            root_bones_nodes.erase(child_node_index);
        }
    }
    GX_ASSERT(root_bones_nodes.size() == 1);
    auto bones_info = load_bone_info(*root_bones_nodes.begin(), context, "", name_to_joint_index, inv_bind_mats);
    joint_index = 0;
    {
        const auto search = name_to_joint_index.find(bones_info.name);
        GX_ASSERT_D(name_to_joint_index.end() != search);
        bone_index_map[search->second] = joint_index;
    }
    set_bone_map(bones_info, name_to_joint_index, bone_index_map, joint_index);
    return { std::move(bone_index_map), std::move(bones_info) };
}

void load_mesh(
    const Context& context,
    const std::uint64_t mesh_index,
    const std::uint64_t primitive_index,
    const core::job::EndCaller<>& end_callback,
    const std::vector<int>& bone_index_map)
{
    const tinygltf::Mesh& msh = context.data.meshes[mesh_index];
    const tinygltf::Primitive& primitive = msh.primitives[primitive_index];
    GX_ASSERT(TINYGLTF_MODE_TRIANGLES == primitive.mode);
    const auto& acs = context.data.accessors;
    const auto& bvs = context.data.bufferViews;
    const auto& bfs = context.data.buffers;
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

    const math::Aabb3 box(
        math::Vec3(pos_max[0], pos_max[1], pos_max[2]),
        math::Vec3(pos_min[0], pos_min[1], pos_min[2]));

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

    core::job::EndCallerShared<mesh::Mesh> mesh_end_callback([ctx = &context, mesh_index, primitive_index, end_callback](std::shared_ptr<mesh::Mesh>&& m) {
        ctx->meshes.meshes[mesh_index]->meshes[primitive_index] = std::move(m);
        (void)end_callback;
    });

    auto name = msh.name + "-primitive:" + std::to_string(primitive_index);
    if (is_animated) {
        engine::Engine::get()->get_mesh_manager()->build(
            std::move(name),
            std::move(animated_vertices),
            std::move(indices),
            box,
            std::shared_ptr(context.materials.materials[primitive.material]),
            std::move(mesh_end_callback));
    } else {
        engine::Engine::get()->get_mesh_manager()->build(
            std::move(name),
            std::move(vertices),
            std::move(indices),
            box,
            std::shared_ptr(context.materials.materials[primitive.material]),
            std::move(mesh_end_callback));
    }
}

void load_mesh(
    const Context& context,
    const int mesh_index,
    const core::job::EndCaller<>& end_callback,
    const std::vector<int>& bone_index_map)
{
    const tinygltf::Mesh& msh = context.data.meshes[mesh_index];
    GX_LOG_D("Loading mesh: " << msh.name);
    context.meshes.meshes[mesh_index]->meshes.resize(msh.primitives.size());
    for (int primitive_index = 0; primitive_index < msh.primitives.size(); ++primitive_index) {
        load_mesh(context, mesh_index, primitive_index, end_callback, bone_index_map);
    }
}
}

gearoenix::render::gltf::Meshes::Meshes(const Context& context)
    : context(context)
{
}

gearoenix::render::gltf::Meshes::~Meshes() = default;

void gearoenix::render::gltf::Meshes::load(core::job::EndCaller<>&& end)
{
    GX_ASSERT_D(meshes.empty());
    meshes.resize(context.data.meshes.size());
    for (int node_index = 0; node_index < context.data.nodes.size(); ++node_index) {
        if (!is_mesh(node_index)) {
            continue;
        }
        const auto mesh_index = context.data.nodes[node_index].mesh;
        core::job::send_job_to_pool([this, mesh_index, node_index, end] {
            meshes[mesh_index] = std::make_unique<Mesh>();
            auto [bone_index_map, bones_info] = process_skin(context, node_index);
            load_mesh(context, mesh_index, end, bone_index_map);
            meshes[mesh_index]->bone_index_map = std::move(bone_index_map);
            meshes[mesh_index]->bones_info = std::move(bones_info);
        });
    }
}

bool gearoenix::render::gltf::Meshes::is_mesh(const int node_index) const
{
    if (const auto& node = context.data.nodes[node_index]; node.mesh != -1) {
        GX_ASSERT_D(node.children.empty());
        GX_ASSERT_D(node.camera == -1);
        GX_ASSERT_D(node.emitter == -1);
        GX_ASSERT_D(node.light == -1);
        return true;
    }
    return false;
}

bool gearoenix::render::gltf::Meshes::process(
    const int node_index,
    physics::Transformation* const parent_transform,
    const core::job::EndCaller<>& end,
    scene::Builder& scene_builder) const
{
    const auto& node = context.data.nodes[node_index];
    if (!is_mesh(node_index)) {
        return false;
    }
    auto model_builder = engine::Engine::get()->get_model_manager()->build(
        std::string(node.name),
        parent_transform,
        std::vector(meshes[node.mesh]->meshes),
        core::job::EndCaller(end),
        true);
    if (meshes[node.mesh]->bone_index_map.size() > 1) {
        engine::Engine::get()->get_physics_engine()->get_animation_manager()->create_armature(
            model_builder->get_entity_builder()->get_builder(),
            meshes[node.mesh]->bones_info.value());
        const auto& skin = context.data.skins[node.skin];
        physics::animation::ArmatureAnimationInfo armature_animation_info;
        for (const auto bone_node_index : skin.joints) {
            auto bone_channel_search = context.animations.bones_channels.find(bone_node_index);
            GX_ASSERT(context.animations.bones_channels.end() != bone_channel_search);
            armature_animation_info.channels.push_back(*bone_channel_search->second);
        }
        armature_animation_info.optimise();
        engine::Engine::get()->get_physics_engine()->get_animation_manager()->create_animation_player(
            model_builder->get_entity_builder()->get_builder(),
            armature_animation_info);
    }
    apply_transform(node_index, context, model_builder->get_transformation());
    scene_builder.add(std::move(model_builder));
    return true;
}
