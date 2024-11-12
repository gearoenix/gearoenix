#include "gx-rnd-cmr-builder.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../physics/collider/gx-phs-cld-frustum.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "gx-rnd-cmr-camera.hpp"

gearoenix::render::camera::Builder::Builder(
    engine::Engine& e, const std::string& name, core::job::EndCaller<>&& entity_in_world_callback,
    physics::TransformationComponent* const parent_transform)
    : entity_builder(e.get_world()->create_shared_builder(std::string(name), std::move(entity_in_world_callback)))
{
    auto& builder = entity_builder->get_builder();
    auto frustum = physics::collider::Frustum::construct(
        name + "-collider",
        physics::collider::Frustum::default_points,
        builder.get_id());
    builder.add_component(std::move(frustum));
    builder.add_component(physics::TransformationComponent::construct(
        name + "-transformation", parent_transform, builder.get_id()));
}

gearoenix::render::camera::Builder::~Builder() = default;

gearoenix::physics::TransformationComponent& gearoenix::render::camera::Builder::get_transformation()
{
    return *entity_builder->get_builder().get_component<physics::TransformationComponent>();
}

const gearoenix::physics::TransformationComponent& gearoenix::render::camera::Builder::get_transformation() const
{
    return *entity_builder->get_builder().get_component<physics::TransformationComponent>();
}

std::shared_ptr<gearoenix::physics::TransformationComponent> gearoenix::render::camera::Builder::get_transformation_shared_ptr() const
{
    return std::dynamic_pointer_cast<physics::TransformationComponent>(get_transformation().get_component_self().lock());
}

gearoenix::physics::collider::Frustum& gearoenix::render::camera::Builder::get_frustum()
{
    return *entity_builder->get_builder().get_component<physics::collider::Frustum>();
}

const gearoenix::physics::collider::Frustum& gearoenix::render::camera::Builder::get_frustum() const
{
    return *entity_builder->get_builder().get_component<physics::collider::Frustum>();
}

gearoenix::render::camera::Camera& gearoenix::render::camera::Builder::get_camera()
{
    return *entity_builder->get_builder().get_component<Camera>();
}

const gearoenix::render::camera::Camera& gearoenix::render::camera::Builder::get_camera() const
{
    return *entity_builder->get_builder().get_component<Camera>();
}

gearoenix::core::ecs::entity_id_t gearoenix::render::camera::Builder::get_id() const
{
    return entity_builder->get_id();
}
