#include "gx-rnd-gltf-node.hpp"
#include "gx-rnd-gltf-context.hpp"
#include "gx-rnd-gltf-transform.hpp"

gearoenix::render::gltf::Nodes::Nodes(const Context& context)
    : context(context)
{
}

gearoenix::render::gltf::Nodes::~Nodes() = default;

bool gearoenix::render::gltf::Nodes::is_empty(const int node_index) const
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
    if (context.armatures.is_armature(node_index)) {
        return false;
    }
    GX_COMPLAIN_D(node.children.size() != 1 || !has_transformation(node_index, context),
        "What is expected here is an empty node for parenting several other entities and "
        "parenting only one child is a waste of transform operation, obviously child can replace this one.");
    return true;
}

bool gearoenix::render::gltf::Nodes::process_empty(
    const int node_index,
    physics::Transformation* const parent_transform,
    const core::job::EndCaller<>& gpu_end_callback,
    const core::job::EndCaller<>& entity_end_callback,
    const std::shared_ptr<scene::Builder>& scene_builder) const
{
    if (!is_empty(node_index)) {
        return false;
    }
    const auto& node = context.data.nodes[node_index];
    const auto transform = create_empty_entity_transform(node_index, context, parent_transform, entity_end_callback, scene_builder);
    for (const auto child : node.children) {
        process(child, transform, gpu_end_callback, entity_end_callback, scene_builder);
    }
    return true;
}

void gearoenix::render::gltf::Nodes::process(
    const int node_index,
    physics::Transformation* const parent_transform,
    const core::job::EndCaller<>& gpu_end_callback,
    const core::job::EndCaller<>& entity_end_callback,
    const std::shared_ptr<scene::Builder>& scene_builder) const
{
    const auto& node = context.data.nodes[node_index];
    GX_LOG_D("Loading node: " << node.name);
    if (context.cameras.process(node_index, parent_transform, gpu_end_callback, entity_end_callback, scene_builder)) {
        return;
    }
    if (context.meshes.process(node_index, parent_transform, entity_end_callback, *scene_builder)) {
        return;
    }
    if (context.lights.process(node_index, parent_transform, gpu_end_callback, entity_end_callback, scene_builder)) {
        return;
    }
    if (context.armatures.process(node_index, parent_transform, gpu_end_callback, entity_end_callback, scene_builder)) {
        return;
    }
    if (process_empty(node_index, parent_transform, gpu_end_callback, entity_end_callback, scene_builder)) {
        return;
    }
    GX_LOG_F("Unexpected node in the scene nodes. Node: " << node.name);
}
