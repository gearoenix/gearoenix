#include "gx-phs-engine.hpp"
#include "../core/ecs/gx-cr-ecs-world.hpp"
#include "../render/engine/gx-rnd-eng-engine.hpp"
#include "animation/gx-phs-anm-manager.hpp"
#include "collider/gx-phs-cld-aabb.hpp"
#include "collider/gx-phs-cld-frustum.hpp"
#include "gx-phs-transformation.hpp"

gearoenix::physics::Engine::Engine(render::engine::Engine& render_engine) noexcept
    : render_engine(render_engine)
    , animation_manager(new animation::Manager(render_engine))
{
}

gearoenix::physics::Engine::~Engine() noexcept = default;

void gearoenix::physics::Engine::start_frame() noexcept
{
    auto* const world = render_engine.get_world();
    world->parallel_system<Transformation, collider::Aabb3>(
        [&](const core::ecs::Entity::id_t, Transformation& transform, collider::Aabb3& cld, const auto /*kernel_index*/) {
            if (transform.get_changed())
                cld.update(transform.get_matrix());
        });
    // No need for updating collider::Frustum, it will be updated in the render::camera::Manager
    world->parallel_system<Transformation>(
        [&](const core::ecs::Entity::id_t, Transformation& transform, const auto /*kernel_index*/) {
            transform.update();
        });
    animation_manager->update();
}

void gearoenix::physics::Engine::end_frame() noexcept
{
    auto* const world = render_engine.get_world();
    world->parallel_system<Transformation>(
        [&](const core::ecs::Entity::id_t, Transformation& transform, const auto /*kernel_index*/) {
            transform.clear();
        });
}
