#include "gx-rnd-scn-manager.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../core/sync/gx-cr-sync-work-waiter.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../gltf/gx-rnd-gltf-loader.hpp"
#include "gx-rnd-scn-scene.hpp"
#include <tiny_gltf.h>

void gearoenix::render::scene::Manager::load_gltf_worker(platform::stream::Path path, const core::sync::EndCallerIgnored& c) noexcept
{
    GX_CHECK_EQUAL_D(nullptr, loader)
    loader = std::make_unique<gltf::Loader>(e, std::move(path));
    const auto& gltf_data = *loader->get_data();
    GX_LOG_D("GLTF default scene is: " << gltf_data.defaultScene)
    auto& scenes = gltf_data.scenes;
    auto& default_gltf_scene = scenes[gltf_data.defaultScene];
    core::ecs::EntityBuilder entity_builder;
    entity_builder.add_component(Scene(e, default_scene, *loader, c));
    default_scene = entity_builder.get_id();
    e->get_world()->delayed_create_entity_with_builder(std::move(entity_builder));
    id_to_name.emplace(default_scene, default_gltf_scene.name);
    name_to_id.emplace(default_gltf_scene.name, default_scene);
}

gearoenix::render::scene::Manager::Manager(engine::Engine* const e) noexcept
    : e(e)
    , io_worker(new core::sync::WorkWaiter())
{
}

gearoenix::render::scene::Manager::~Manager() noexcept = default;

void gearoenix::render::scene::Manager::load_gltf(
    platform::stream::Path path, core::sync::EndCallerIgnored call) noexcept
{
    io_worker->push([this, p = std::move(path), c = std::move(call)]() mutable noexcept {
        load_gltf_worker(std::move(p), c);
    });
}
