#include "gx-phs-cns-constraint.hpp"

gearoenix::physics::constraint::Constraint::Constraint(
    const core::ecs::component_index_t final_type_index,
    std::string&& name,
    const core::ecs::entity_id_t entity_id)
    : Component(final_type_index, std::move(name), entity_id)
{
}

gearoenix::physics::constraint::Constraint::~Constraint() = default;

void gearoenix::physics::constraint::Constraint::update()
{
}