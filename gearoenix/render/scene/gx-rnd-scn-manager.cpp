#include "gx-rnd-scn-manager.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "gx-rnd-scn-builder.hpp"
#include "gx-rnd-scn-scene.hpp"

gearoenix::render::scene::Manager::Manager(engine::Engine& e) noexcept
    : e(e)
{
}

gearoenix::render::scene::Manager::~Manager() noexcept = default;

std::shared_ptr<gearoenix::render::scene::Builder> gearoenix::render::scene::Manager::build(
    const std::string& name, const double layer, core::sync::EndCaller&& end_callback) noexcept
{
    return std::shared_ptr<Builder>(new Builder(e, name, layer, std::move(end_callback)));
}

void gearoenix::render::scene::Manager::update() noexcept
{
    e.get_world()->parallel_system<Scene>([](const core::ecs::Entity::id_t scene_id, Scene* const s, const auto /*kernel_index*/) noexcept {
        s->update(scene_id);
    });
}