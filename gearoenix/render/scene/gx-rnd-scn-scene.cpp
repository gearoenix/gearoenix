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

gearoenix::render::scene::Scene::~Scene() noexcept = default;

gearoenix::render::scene::Scene::Scene(Scene&&) noexcept = default;

void gearoenix::render::scene::Scene::update() noexcept
{
}
