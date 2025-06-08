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

bool gearoenix::render::gltf::Nodes::process_empty(const int node_index, core::ecs::Entity* const parent, const core::job::EndCaller<>& end_callback) const
{
    if (!is_empty(node_index)) {
        return false;
    }
    const auto& node = context.data.nodes[node_index];
    auto empty_entity = create_empty_entity_transform(node_index, context, parent);
    for (const auto child : node.children) {
        process(child, empty_entity.get(), end_callback);
    }
    return true;
}

void gearoenix::render::gltf::Nodes::process(
    const int node_index,
    core::ecs::Entity* const parent,
    const core::job::EndCaller<>& end_callback) const
{
    const auto& node = context.data.nodes[node_index];
    GX_LOG_D("Loading node: " << node.name);
    if (context.cameras.process(node_index, parent, end_callback)) {
        return;
    }
    if (context.meshes.process(node_index, parent)) {
        return;
    }
    if (context.lights.process(node_index, parent, end_callback)) {
        return;
    }
    if (context.armatures.process(node_index, parent, end_callback)) {
        return;
    }
    if (process_empty(node_index, parent, end_callback)) {
        return;
    }
    GX_LOG_F("Unexpected node in the scene nodes. Node: " << node.name);
}
