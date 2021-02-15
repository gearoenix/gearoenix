#include "gx-rnd-gltf-node-manager.hpp"
#include "gx-rnd-gltf-loader.hpp"
#include <tiny_gltf.h>

gearoenix::core::ecs::Entity::id_t gearoenix::render::gltf::NodeManager::get_mesh(
    const std::size_t index, const gearoenix::core::sync::EndCallerIgnored& c) noexcept
{
    const auto& data = *loader.get_data();
    const auto& nodes = data.nodes;
    const auto& node = nodes[index];
    const auto& mesh = data.meshes[node.mesh];
    mesh.
}

gearoenix::core::ecs::Entity::id_t gearoenix::render::gltf::NodeManager::get_camera(
    const std::size_t, const gearoenix::core::sync::EndCallerIgnored&) noexcept {
    GX_UNIMPLEMENTED
}

gearoenix::core::ecs::Entity::id_t gearoenix::render::gltf::NodeManager::get_camera(
    const std::size_t, const std::size_t, const gearoenix::core::sync::EndCallerIgnored&) noexcept {
    GX_UNIMPLEMENTED
}

gearoenix::core::ecs::Entity::id_t gearoenix::render::gltf::NodeManager::get_skin(
    const std::size_t, const gearoenix::core::sync::EndCallerIgnored&) noexcept {
    GX_UNIMPLEMENTED
}

gearoenix::core::ecs::Entity::id_t gearoenix::render::gltf::NodeManager::get_light(
    const std::size_t, const std::size_t, const gearoenix::core::sync::EndCallerIgnored&) noexcept {
    GX_UNIMPLEMENTED
}

gearoenix::render::gltf::NodeManager::NodeManager(Loader& loader) noexcept
    : loader(loader)
{
}

gearoenix::core::ecs::Entity::id_t gearoenix::render::gltf::NodeManager::get(
    const std::size_t index, const core::sync::EndCallerIgnored& c) noexcept
{
    auto index_search = loaded_nodes.find(index);
    if (loaded_nodes.end() != index_search)
        return index_search->second;

    const auto& data = *loader.get_data();
    const auto& nodes = data.nodes;
    const auto& node = nodes[index];

    core::ecs::Entity::Builder entity_builder;

    const auto id = entity_builder.get_id();
    entity_builder.set_name(node.name);

    loaded_nodes.emplace(index, entity_builder.get_id());

    if (node.mesh >= 0) {
        get_mesh(index, std::move(entity_builder), c);
        return id;
    }

    if (node.camera >= 0)
        return get_camera(index, c);

    if (node.skin >= 0)
        return get_skin(index, c);

    GX_CHECK_NOT_EQUAL_D(node.children.size(), 1)

    const std::size_t child_node_index = node.children[0];
    auto child_node_search = loaded_nodes.find(child_node_index);
    GX_CHECK_EQUAL_D(loaded_nodes.end(), child_node_search)
    const auto& child_node = nodes[child_node_index];

    if (child_node.camera >= 0)
        return get_camera(index, child_node_index, c);

    GX_CHECK_EQUAL_D(child_node.mesh, -1)
    GX_CHECK_EQUAL_D(child_node.skin, -1)

    if (child_node.extensions.contains("KHR_lights_punctual"))
        return get_light(index, child_node_index, c);

    GX_UNEXPECTED
}
