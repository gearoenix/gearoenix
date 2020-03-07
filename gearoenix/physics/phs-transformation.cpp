#include "phs-transformation.hpp"

gearoenix::physics::Transformation::Transformation() noexcept
    : x_axis(1.0, 0.0, 0.0)
    , y_axis(0.0, 1.0, 0.0)
    , z_axis(0.0, 0.0, 1.0)
    , current_local_scale(1.0)
    , on_scale([] {})
{
}

gearoenix::physics::Transformation::~Transformation() noexcept = default;

void gearoenix::physics::Transformation::get_location(math::Vec3<double>&) const noexcept
{
    GXUNIMPLEMENTED
}

const gearoenix::math::Vec3<double>& gearoenix::physics::Transformation::get_location() const noexcept
{
    GXUNIMPLEMENTED
}

void gearoenix::physics::Transformation::set_location(const math::Vec3<double>&) noexcept
{
    GXUNIMPLEMENTED
}

void gearoenix::physics::Transformation::translate(const math::Vec3<double>&) noexcept
{
    GXUNIMPLEMENTED
}

void gearoenix::physics::Transformation::local_x_translate(const double) noexcept
{
    GXUNIMPLEMENTED
}

void gearoenix::physics::Transformation::local_y_translate(const double) noexcept
{
    GXUNIMPLEMENTED
}

void gearoenix::physics::Transformation::local_z_translate(const double) noexcept
{
    GXUNIMPLEMENTED
}

void gearoenix::physics::Transformation::global_rotate(const double, const math::Vec3<double>&, const math::Vec3<double>&) noexcept
{
    GXUNIMPLEMENTED
}

void gearoenix::physics::Transformation::local_rotate(const double, const math::Vec3<double>&) noexcept
{
    GXUNIMPLEMENTED
}

void gearoenix::physics::Transformation::local_x_rotate(const double) noexcept
{
    GXUNIMPLEMENTED
}

void gearoenix::physics::Transformation::local_y_rotate(const double) noexcept
{
    GXUNIMPLEMENTED
}

void gearoenix::physics::Transformation::local_z_rotate(const double) noexcept
{
    GXUNIMPLEMENTED
}

void gearoenix::physics::Transformation::local_scale(const double) noexcept
{
    GXUNIMPLEMENTED
}

void gearoenix::physics::Transformation::local_x_scale(const double) noexcept
{
    GXUNIMPLEMENTED
}

void gearoenix::physics::Transformation::local_y_scale(const double) noexcept
{
    GXUNIMPLEMENTED
}

void gearoenix::physics::Transformation::local_z_scale(const double) noexcept
{
    GXUNIMPLEMENTED
}

void gearoenix::physics::Transformation::set_orientation(const math::Quat<double>&) noexcept
{
    GXUNIMPLEMENTED
}
