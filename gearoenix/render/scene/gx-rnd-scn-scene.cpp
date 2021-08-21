#include "gx-rnd-scn-scene.hpp"
#include "../gltf/gx-rnd-gltf-loader.hpp"
#include "../gltf/gx-rnd-gltf-node-manager.hpp"
#include <tiny_gltf.h>

gearoenix::render::scene::Scene::Scene(
    engine::Engine* const,
    const std::size_t scene_index,
    gltf::Loader& loader,
    const core::sync::EndCallerIgnored& c) noexcept
    : core::ecs::Component(this)
{
    const auto& node_indices = loader.get_data()->scenes[scene_index].nodes;
    for (const auto node_index : node_indices) {
        entities.push_back(loader.get_node_manager()->get(node_index, c));
    }
    std::sort(entities.begin(), entities.end());
}

gearoenix::render::scene::Scene::Scene() noexcept
    : core::ecs::Component(this)
{
}

gearoenix::render::scene::Scene::~Scene() noexcept = default;

gearoenix::render::scene::Scene::Scene(Scene&&) noexcept = default;

void gearoenix::render::scene::Scene::add_mesh(const core::ecs::Entity::id_t entity) noexcept
{
    mesh_entities.insert(std::upper_bound(mesh_entities.begin(), mesh_entities.end(), entity), entity);
}

void gearoenix::render::scene::Scene::add_camera(const core::ecs::Entity::id_t entity) noexcept
{
    camera_entities.insert(std::upper_bound(camera_entities.begin(), camera_entities.end(), entity), entity);
}

void gearoenix::render::scene::Scene::update() noexcept
{
}