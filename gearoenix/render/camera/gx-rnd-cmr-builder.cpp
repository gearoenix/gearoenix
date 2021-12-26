#include "gx-rnd-cmr-builder.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"

gearoenix::render::camera::Builder::Builder(engine::Engine& e, const std::string& name) noexcept
    : entity_builder(e.get_world()->create_shared_builder())
{
    auto& builder = entity_builder->get_builder();
    builder.set_name(name);
    builder.add_component(physics::Transformation());
}

gearoenix::render::camera::Builder::~Builder() noexcept = default;

void gearoenix::render::camera::Builder::set(Projection) noexcept
{
}

gearoenix::physics::Transformation& gearoenix::render::camera::Builder::get_transformation() noexcept
{
    return *entity_builder->get_builder().get_component<physics::Transformation>();
}

const gearoenix::physics::Transformation& gearoenix::render::camera::Builder::get_transformation() const noexcept
{
    return *entity_builder->get_builder().get_component<physics::Transformation>();
}
