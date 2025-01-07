#include "gx-rnd-gltf-armature.hpp"
#include "../../core/ecs/gx-cr-ecs-entity-builder.hpp"
#include "../../physics/animation/gx-phs-anm-armature.hpp"
#include "../../physics/animation/gx-phs-anm-bone-builder.hpp"
#include "../../physics/animation/gx-phs-anm-manager.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../scene/gx-rnd-scn-builder.hpp"
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

bool gearoenix::render::gltf::Armatures::process(
    const int node_index,
    physics::Transformation* const parent_transform,
    const core::job::EndCaller<>& gpu_end_callback,
    const core::job::EndCaller<>& entity_end_callback,
    const std::shared_ptr<scene::Builder>& scene_builder) const
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
    auto bone = std::shared_ptr(context.animations.bone_builders.find(bone_node_index)->second->bone);

    auto entity_builder = physics::animation::Manager::get()->create_armature_builder(
        std::string(node.name), parent_transform, std::move(bone), core::job::EndCaller(entity_end_callback));
    const auto& builder = entity_builder->get_builder();
    auto* const transform = builder.get_component<physics::Transformation>();
    apply_transform(node_index, context, *transform);
    context.nodes.process(mesh_node_index, transform, gpu_end_callback, entity_end_callback, scene_builder);
    auto* const arm = builder.get_component<physics::animation::Armature>();
    const auto& bone_nodes = context.data.skins[context.data.nodes[mesh_node_index].skin].joints;
    auto& arm_bones = arm->get_all_bones();
    std::vector<int> bone_indices;
    bone_indices.reserve(bone_nodes.size());
    for (const auto bone_index : bone_nodes) {
        auto* const ptr = context.animations.bone_builders.find(bone_index)->second->bone.get();
        bone_indices.push_back(static_cast<int>(std::distance(arm_bones.begin(), std::ranges::find(arm_bones, ptr))));
    }
    arm->sort_all_bones(bone_indices);
    scene_builder->add_armature(std::move(entity_builder));
    return true;
}
