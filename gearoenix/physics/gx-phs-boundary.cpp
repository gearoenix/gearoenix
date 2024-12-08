#include "gx-phs-boundary.hpp"
#include "../core/ecs/gx-cr-ecs-comp-type.hpp"

gearoenix::physics::Boundary::Boundary(
    const math::Vec3<double>& upper,
    const math::Vec3<double>& lower,
    std::string&& name,
    const core::ecs::entity_id_t entity_id)
    : Component(core::ecs::ComponentType::create_index(this), std::move(name), entity_id)
    , box(upper, lower)
{
}