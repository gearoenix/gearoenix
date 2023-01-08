#include "gx-phs-boundary.hpp"

gearoenix::physics::Boundary::Boundary(const math::Vec3<double>& upper, const math::Vec3<double>& lower, std::string&& name) noexcept
    : core::ecs::Component(this, std::move(name))
    , box(upper, lower)
{
}

gearoenix::physics::Boundary::Boundary(Boundary&&) noexcept = default;
