#include "gx-phs-boundary.hpp"

gearoenix::physics::Boundary::Boundary(const math::Vec3<double>& upper, const math::Vec3<double>& lower) noexcept
    : core::ecs::Component(this)
    , box(upper, lower)
{
}

gearoenix::physics::Boundary::Boundary(Boundary&&) noexcept = default;
