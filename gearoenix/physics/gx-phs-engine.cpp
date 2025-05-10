#include "gx-phs-engine.hpp"
#include "../core/ecs/gx-cr-ecs-world.hpp"
#include "../render/engine/gx-rnd-eng-engine.hpp"
#include "animation/gx-phs-anm-bone.hpp"
#include "animation/gx-phs-anm-manager.hpp"
#include "collider/gx-phs-cld-aabb.hpp"
#include "collider/gx-phs-cld-frustum.hpp"
#include "constraint/gx-phs-cns-manager.hpp"
#include "gx-phs-transformation.hpp"

gearoenix::physics::Engine::Engine()
    : Singleton(this)
    , animation_manager(new animation::Manager())
    , constraint_manager(new constraint::Manager())
{
    core::ecs::ComponentType::add<collider::Collider>();
    core::ecs::ComponentType::add<collider::Aabb3>();
    core::ecs::ComponentType::add<collider::Frustum>();
    core::ecs::ComponentType::add<Transformation>();
}

gearoenix::physics::Engine::~Engine() = default;

void gearoenix::physics::Engine::start_frame()
{
    animation_manager->update();
    constraint_manager->update();
    Transformation::update();
    animation::Bone::update_all_bones_after_transform_updates();
    collider::Collider::update_all_after_transform_update();
}

void gearoenix::physics::Engine::end_frame()
{
    core::ecs::World::get().parallel_system<Transformation>([&](const auto, auto* const transform, const auto) {
        transform->clear_change();
    });
}
