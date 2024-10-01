#include "gx-phs-boundary.hpp"

gearoenix::physics::Boundary::Boundary(const math::Vec3<double>& upper, const math::Vec3<double>& lower, std::string&& name)
    : Component(create_this_type_index(this), std::move(name))
    , box(upper, lower)
{
}

const gearoenix::core::ecs::Component::HierarchyTypes& gearoenix::physics::Boundary::get_hierarchy_types() const
{
    static const auto types = generate_hierarchy_types(this);
    return types;
}
