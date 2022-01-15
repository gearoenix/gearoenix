#include "gx-phs-engine.hpp"
#include "../core/ecs/gx-cr-ecs-world.hpp"
#include "../render/engine/gx-rnd-eng-engine.hpp"
#include "gx-phs-transformation.hpp"

gearoenix::physics::Engine::Engine(render::engine::Engine& render_engine) noexcept
    : render_engine(render_engine)
{
}

void gearoenix::physics::Engine::update() noexcept
{
    auto* const world = render_engine.get_world();
    world->parallel_system<Transformation>(
        [&](const core::ecs::Entity::id_t, Transformation& transform, const unsigned int) {
            transform.update();
        });
}
