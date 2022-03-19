#include "gx-rnd-gltf-node-manager.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../gx-rnd-vertex.hpp"
#include "gx-rnd-gltf-loader.hpp"
#include "gx-rnd-gltf-mesh-manager.hpp"
#include <tiny_gltf.h>

gearoenix::core::ecs::Entity::id_t gearoenix::render::gltf::NodeManager::get_camera(
    const std::size_t, const gearoenix::core::sync::EndCallerIgnored&) noexcept
{
    GX_TODO;
    return 0;
}

gearoenix::core::ecs::Entity::id_t gearoenix::render::gltf::NodeManager::get_camera(
    const std::size_t, const std::size_t, const gearoenix::core::sync::EndCallerIgnored&) noexcept
{
    GX_TODO;
    return 0;
}

gearoenix::core::ecs::Entity::id_t gearoenix::render::gltf::NodeManager::get_skin(
    const std::size_t, const gearoenix::core::sync::EndCallerIgnored&) noexcept
{
    GX_TODO;
    return 0;
}

gearoenix::core::ecs::Entity::id_t gearoenix::render::gltf::NodeManager::get_light(
    const std::size_t, const std::size_t, const gearoenix::core::sync::EndCallerIgnored&) noexcept
{
    GX_TODO;
    return 0;
}

gearoenix::render::gltf::NodeManager::NodeManager(Loader& loader, MeshManager& mesh_manager) noexcept
    : loader(loader)
    , mesh_manager(mesh_manager)
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

    auto shared_entity_builder = loader.get_e()->get_world()->create_shared_builder();
    auto& entity_builder = shared_entity_builder->get_builder();

    const auto id = entity_builder.get_id();
    entity_builder.set_name(node.name);

    physics::Transformation transformation;

    if (!node.scale.empty()) {
        const auto& s = node.scale;
        GX_CHECK_EQUAL_D(s.size(), 3);
        transformation.local_scale(math::Vec3(s[0], s[1], s[2]));
    }

    if (!node.rotation.empty()) {
        const auto& r = node.rotation;
        GX_CHECK_EQUAL_D(r.size(), 4);
        transformation.set_orientation(math::Quat(r[0], r[1], r[2], r[3]));
    }

    if (!node.translation.empty()) {
        const auto& t = node.translation;
        GX_CHECK_EQUAL_D(t.size(), 3);
        transformation.translate(math::Vec3(t[0], t[1], t[2]));
    }

    entity_builder.add_component(std::move(transformation));

    loaded_nodes.emplace(index, id);

    if (node.mesh >= 0) {
        mesh_manager.get(node.mesh, std::move(shared_entity_builder), c);
        return id;
    }

    if (node.camera >= 0)
        return get_camera(index, c);

    if (node.skin >= 0)
        return get_skin(index, c);

    GX_CHECK_EQUAL_D(node.children.size(), 1);

    const std::size_t child_node_index = node.children[0];
    GX_CHECK_EQUAL_D(loaded_nodes.end(), loaded_nodes.find(child_node_index));
    const auto& child_node = nodes[child_node_index];

    if (child_node.camera >= 0)
        return get_camera(index, child_node_index, c);

    GX_CHECK_EQUAL_D(child_node.mesh, -1);
    GX_CHECK_EQUAL_D(child_node.skin, -1);

    if (child_node.extensions.contains("KHR_lights_punctual"))
        return get_light(index, child_node_index, c);

    GX_UNEXPECTED;
}
