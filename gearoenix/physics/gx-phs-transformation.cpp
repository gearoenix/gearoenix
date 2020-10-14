#include "gx-phs-transformation.hpp"

gearoenix::physics::Transformation::Transformation() noexcept
    : x_axis(1.0, 0.0, 0.0)
    , y_axis(0.0, 1.0, 0.0)
    , z_axis(0.0, 0.0, 1.0)
    , current_local_scale(1.0)
    , on_scale([] {})
{
}

gearoenix::physics::Transformation::~Transformation() noexcept = default;

void gearoenix::physics::Transformation::get_location(math::Vec3<double>&) const noexcept {
    GX_UNIMPLEMENTED
}

gearoenix::math::Vec3<double> gearoenix::physics::Transformation::get_location() const noexcept
{
    GX_UNIMPLEMENTED
}

void gearoenix::physics::Transformation::set_location(const math::Vec3<double>&) noexcept
{
    GX_UNIMPLEMENTED
}

void gearoenix::physics::Transformation::translate(const math::Vec3<double>&) noexcept
{
    GX_UNIMPLEMENTED
}

void gearoenix::physics::Transformation::local_x_translate(const double) noexcept
{
    GX_UNIMPLEMENTED
}

void gearoenix::physics::Transformation::local_y_translate(const double) noexcept
{
    GX_UNIMPLEMENTED
}

void gearoenix::physics::Transformation::local_z_translate(const double) noexcept
{
    GX_UNIMPLEMENTED
}

void gearoenix::physics::Transformation::global_rotate(const double, const math::Vec3<double>&, const math::Vec3<double>&) noexcept
{
    GX_UNIMPLEMENTED
}

void gearoenix::physics::Transformation::local_rotate(const double, const math::Vec3<double>&) noexcept
{
    GX_UNIMPLEMENTED
}

void gearoenix::physics::Transformation::local_x_rotate(const double) noexcept
{
    GX_UNIMPLEMENTED
}

void gearoenix::physics::Transformation::local_y_rotate(const double) noexcept
{
    GX_UNIMPLEMENTED
}

void gearoenix::physics::Transformation::local_z_rotate(const double) noexcept
{
    GX_UNIMPLEMENTED
}

void gearoenix::physics::Transformation::local_scale(const double) noexcept
{
    GX_UNIMPLEMENTED
}

void gearoenix::physics::Transformation::local_scale(const math::Vec3<double>& s) noexcept
{
    local_x_scale(s.x);
    local_y_scale(s.y);
    local_z_scale(s.z);
}

void gearoenix::physics::Transformation::local_x_scale(const double) noexcept
{
    GX_UNIMPLEMENTED
}

void gearoenix::physics::Transformation::local_y_scale(const double) noexcept
{
    GX_UNIMPLEMENTED
}

void gearoenix::physics::Transformation::local_z_scale(const double) noexcept
{
    GX_UNIMPLEMENTED
}

void gearoenix::physics::Transformation::set_orientation(const math::Quat<double>&) noexcept
{
    GX_UNIMPLEMENTED
}
