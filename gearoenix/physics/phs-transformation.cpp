#include "phs-transformation.hpp"
#include "../system/sys-log.hpp"

gearoenix::physics::Transformation::Transformation() noexcept
    : x_axis(1.0f, 0.0f, 0.0f)
    , y_axis(0.0f, 1.0f, 0.0f)
    , z_axis(0.0f, 0.0f, 1.0f)
    , current_local_scale(1.0f)
    , on_scale([] {})
{
}

gearoenix::physics::Transformation::~Transformation() noexcept = default;

void gearoenix::physics::Transformation::get_location(math::Vec3&) const noexcept
{
    GXUNIMPLEMENTED
}

const gearoenix::math::Vec3& gearoenix::physics::Transformation::get_location() const noexcept
{
    GXUNIMPLEMENTED
}

void gearoenix::physics::Transformation::set_location(const math::Vec3&) noexcept
{
    GXUNIMPLEMENTED
}

void gearoenix::physics::Transformation::translate(const math::Vec3&) noexcept
{
    GXUNIMPLEMENTED
}

void gearoenix::physics::Transformation::local_x_translate(const core::Real) noexcept
{
    GXUNIMPLEMENTED
}

void gearoenix::physics::Transformation::local_y_translate(const core::Real) noexcept
{
    GXUNIMPLEMENTED
}

void gearoenix::physics::Transformation::local_z_translate(const core::Real) noexcept
{
    GXUNIMPLEMENTED
}

void gearoenix::physics::Transformation::global_rotate(const core::Real, const math::Vec3&, const math::Vec3&) noexcept
{
    GXUNIMPLEMENTED
}

void gearoenix::physics::Transformation::local_rotate(const core::Real, const math::Vec3&) noexcept
{
    GXUNIMPLEMENTED
}

void gearoenix::physics::Transformation::local_x_rotate(const core::Real) noexcept
{
    GXUNIMPLEMENTED
}

void gearoenix::physics::Transformation::local_y_rotate(const core::Real) noexcept
{
    GXUNIMPLEMENTED
}

void gearoenix::physics::Transformation::local_z_rotate(const core::Real) noexcept
{
    GXUNIMPLEMENTED
}

void gearoenix::physics::Transformation::local_scale(const core::Real) noexcept
{
    GXUNIMPLEMENTED
}

void gearoenix::physics::Transformation::local_x_scale(const core::Real) noexcept
{
    GXUNIMPLEMENTED
}

void gearoenix::physics::Transformation::local_y_scale(const core::Real) noexcept
{
    GXUNIMPLEMENTED
}

void gearoenix::physics::Transformation::local_z_scale(const core::Real) noexcept
{
    GXUNIMPLEMENTED
}

void gearoenix::physics::Transformation::set_orientation(const math::Quat&) noexcept
{
    GXUNIMPLEMENTED
}
