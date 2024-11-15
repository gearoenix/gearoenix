#include "gx-phs-engine.hpp"
#include "../core/ecs/gx-cr-ecs-world.hpp"
#include "../render/engine/gx-rnd-eng-engine.hpp"
#include "animation/gx-phs-anm-manager.hpp"
#include "collider/gx-phs-cld-aabb.hpp"
#include "collider/gx-phs-cld-frustum.hpp"
#include "constraint/gx-phs-cns-manager.hpp"
#include "gx-phs-transformation.hpp"

gearoenix::physics::Engine::Engine(render::engine::Engine& render_engine)
    : render_engine(render_engine)
    , animation_manager(new animation::Manager(render_engine))
    , constraint_manager(new constraint::Manager(render_engine))
{
}

gearoenix::physics::Engine::~Engine() = default;

void gearoenix::physics::Engine::start_frame()
{
    auto* const world = render_engine.get_world();
    constraint_manager->update();
    Transformation::update(world);
    world->parallel_system<core::ecs::All<Transformation, collider::Aabb3>>(
        [&](const auto, const auto* const transform, auto* const cld, const auto) {
            if (transform->get_changed()) {
                cld->update(transform->get_global_matrix());
            }
        });
    animation_manager->update();
}

void gearoenix::physics::Engine::end_frame()
{
    auto* const world = render_engine.get_world();
    world->parallel_system<Transformation>([&](const auto, auto* const transform, const auto) {
        transform->clear_change();
    });
}
