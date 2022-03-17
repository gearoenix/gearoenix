#include "gx-phs-engine.hpp"
#include "../core/ecs/gx-cr-ecs-world.hpp"
#include "../render/engine/gx-rnd-eng-engine.hpp"
#include "gx-phs-transformation.hpp"
#include "collider/gx-phs-cld-aabb.hpp"

gearoenix::physics::Engine::Engine(render::engine::Engine& render_engine) noexcept
    : render_engine(render_engine)
{
}

void gearoenix::physics::Engine::update() noexcept
{
    auto* const world = render_engine.get_world();
    world->parallel_system<Transformation, collider::Aabb3>(
        [&](const core::ecs::Entity::id_t, Transformation& transform, collider::Aabb3& cld, const unsigned int) {
            if(transform.get_changed())
                cld.update(transform.get_matrix());
        });
    world->parallel_system<Transformation>(
        [&](const core::ecs::Entity::id_t, Transformation& transform, const unsigned int) {
            transform.update();
        });
}
