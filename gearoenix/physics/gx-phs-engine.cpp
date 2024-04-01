#include "gx-phs-engine.hpp"
#include "../core/ecs/gx-cr-ecs-world.hpp"
#include "../render/engine/gx-rnd-eng-engine.hpp"
#include "animation/gx-phs-anm-manager.hpp"
#include "collider/gx-phs-cld-aabb.hpp"
#include "collider/gx-phs-cld-frustum.hpp"
#include "gx-phs-transformation.hpp"

gearoenix::physics::Engine::Engine(render::engine::Engine& render_engine)
    : render_engine(render_engine)
    , animation_manager(new animation::Manager(render_engine))
{
}

gearoenix::physics::Engine::~Engine() = default;

void gearoenix::physics::Engine::start_frame()
{
    auto* const world = render_engine.get_world();
    world->parallel_system<core::ecs::Any<core::ecs::All<Transformation, collider::Aabb3>, Transformation>>(
        [&](const core::ecs::entity_id_t, Transformation* const transform, collider::Aabb3* const cld, const auto /*kernel_index*/) {
            if (nullptr != cld && transform->get_changed())
                cld->update(transform->get_local_matrix()); // TODO: Later, when I developed the parenting constraint, it must change to global_matrix
            transform->local_update(); // TODO: Later, when I developed the parenting constraint, it must change to global_matrix
            // No need for updating collider::Frustum, it will be updated in the render::camera::Manager
        });
    animation_manager->update();
}

void gearoenix::physics::Engine::end_frame()
{
    auto* const world = render_engine.get_world();
    world->parallel_system<Transformation>(
        [&](const core::ecs::entity_id_t, Transformation* const transform, const auto /*kernel_index*/) {
            transform->clear_change();
        });
}
