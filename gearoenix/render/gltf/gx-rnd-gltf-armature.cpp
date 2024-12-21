#include "gx-rnd-gltf-armature.hpp"
#include "gx-rnd-gltf-context.hpp"
#include "gx-rnd-gltf-transform.hpp"

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
    auto* const transform = create_empty_entity_transform(node_index, context, parent_transform, entity_end_callback, scene_builder);
    if (const auto mesh_node_index = node.children[0]; context.skins.is_skin(mesh_node_index)) {
        context.nodes.process(mesh_node_index, transform, gpu_end_callback, entity_end_callback, scene_builder);
        return true;
    }
    if (const auto mesh_node_index = node.children[1]; context.skins.is_skin(mesh_node_index)) {
        context.nodes.process(mesh_node_index, transform, gpu_end_callback, entity_end_callback, scene_builder);
        return true;
    }
    GX_LOG_F("The correct node for skin and mesh not found. Node: " << node.name);
}
