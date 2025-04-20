#include "gx-phs-cns-constraint.hpp"

gearoenix::physics::constraint::Constraint::Constraint(
    const core::object_type_index_t final_type_index, std::string&& name)
    : Component(final_type_index, std::move(name))
{
}

gearoenix::physics::constraint::Constraint::~Constraint() = default;

void gearoenix::physics::constraint::Constraint::update()
{
}