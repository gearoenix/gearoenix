#include "gx-phs-boundary.hpp"

gearoenix::physics::Boundary::Boundary(const math::Vec3<double>& upper, const math::Vec3<double>& lower, std::string&& name)
    : core::ecs::Component(core::ecs::Component::create_this_type_index(this), std::move(name))
    , box(upper, lower)
{
}
