#include "gx-rnd-gltf-armature.hpp"
#include "../../physics/animation/gx-phs-anm-armature.hpp"
#include "../../physics/animation/gx-phs-anm-bone.hpp"
#include "../../physics/animation/gx-phs-anm-manager.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../scene/gx-rnd-scn-scene.hpp"
#include "gx-rnd-gltf-context.hpp"
#include "gx-rnd-gltf-transform.hpp"
#include <algorithm>

gearoenix::render::gltf::Armatures::Armatures(const Context& context)
    : context(context)
{
}

gearoenix::render::gltf::Armatures::~Armatures() = default;

bool gearoenix::render::gltf::Armatures::is_armature(const int node_index) const
{
    const auto& node = context.data.nodes[node_index];
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
    const auto first_skin = context.skins.is_skin(node.children[0]);
    const auto second_skin = context.skins.is_skin(node.children[1]);
    if (!first_skin && !second_skin) {
        return false;
    }
    GX_ASSERT_D(first_skin != second_skin);
    return true;
}

bool gearoenix::render::gltf::Armatures::process(const int node_index, core::ecs::Entity* const parent, const core::job::EndCaller<>& end_callback) const
{
    const auto& node = context.data.nodes[node_index];
    if (!is_armature(node_index)) {
        return false;
    }
    auto mesh_node_index = node.children[0];
    int bone_node_index;
    if (context.skins.is_skin(mesh_node_index)) {
        bone_node_index = node.children[1];
    } else if (mesh_node_index = node.children[1]; context.skins.is_skin(mesh_node_index)) {
        bone_node_index = node.children[0];
    } else {
        GX_LOG_F("The correct node for skin and mesh not found. Node: " << node.name);
    }
    auto bone = context.animations.bone_entities.find(bone_node_index)->second->get_component_shared_ptr<physics::animation::Bone>();

    auto entity = physics::animation::Manager::get()->create_armature(std::string(node.name), parent, std::move(bone));
    auto* const transform = entity->get_component<physics::Transformation>();
    apply_transform(node_index, context, *transform);
    context.nodes.process(mesh_node_index, entity.get(), end_callback);
    auto* const arm = entity->get_component<physics::animation::Armature>();
    const auto skin_index = context.data.nodes[mesh_node_index].skin;
    GX_ASSERT_D(skin_index != -1);
    const auto& skin = context.data.skins[skin_index];
    const auto& bone_nodes = skin.joints;
    auto& arm_bones = arm->get_all_bones();
    std::vector<int> bone_indices;
    bone_indices.reserve(bone_nodes.size());
    for (const auto bone_index : bone_nodes) {
        auto* const ptr = context.animations.bone_entities.find(bone_index)->second->get_component<physics::animation::Bone>();
        bone_indices.push_back(static_cast<int>(std::distance(arm_bones.begin(), std::ranges::find(arm_bones, ptr))));
    }
    arm->sort_all_bones(bone_indices);
    std::vector<math::Mat4x4<double>> inverse_bind_matrices;
    const auto inv_mat_index = skin.inverseBindMatrices;
    GX_ASSERT_D(inv_mat_index != -1);
    const auto& inv_mat_acc = context.data.accessors[inv_mat_index];
    GX_ASSERT_D(inv_mat_acc.bufferView >= 0);
    GX_ASSERT_D(inv_mat_acc.count > 0);
    inverse_bind_matrices.resize(inv_mat_acc.count);
    GX_ASSERT_D(inv_mat_acc.type == TINYGLTF_TYPE_MAT4);
    GX_ASSERT_D(inv_mat_acc.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
    const auto& inv_mat_bv = context.data.bufferViews[inv_mat_acc.bufferView];
    GX_ASSERT_D(inv_mat_bv.buffer >= 0);
    const auto* const mat_b = &context.data.buffers[inv_mat_bv.buffer].data[inv_mat_bv.byteOffset];
    const auto mat_bi_inc = inv_mat_bv.byteStride > 0 ? inv_mat_bv.byteStride : sizeof(math::Mat4x4<float>);
    auto mat_bi = decltype(mat_bi_inc) { 0 };
    for (auto& mat : inverse_bind_matrices) {
        mat = math::Mat4x4<double>(*reinterpret_cast<const math::Mat4x4<float>*>(&mat_b[mat_bi]));
        mat_bi += mat_bi_inc;
    }
    for (int i = 0; i < inv_mat_acc.count; ++i) {
        arm->get_all_bones()[i]->set_inverse_bind_matrix(inverse_bind_matrices[i]);
    }
    return true;
}
