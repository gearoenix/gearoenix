#include "phs-transformation.hpp"
#include "../system/sys-log.hpp"

gearoenix::physics::Transformation::Transformation() noexcept
{
}

gearoenix::physics::Transformation::~Transformation() noexcept
{
}

const gearoenix::math::Vec3& gearoenix::physics::Transformation::get_x_axis() const noexcept
{
    GXUNIMPLEMENTED;
}

const gearoenix::math::Vec3& gearoenix::physics::Transformation::get_y_axis() const noexcept
{
    GXUNIMPLEMENTED;
}

const gearoenix::math::Vec3& gearoenix::physics::Transformation::get_z_axis() const noexcept
{
    GXUNIMPLEMENTED;
}

void gearoenix::physics::Transformation::get_location(math::Vec3&) const noexcept
{
    GXUNIMPLEMENTED;
}

const gearoenix::math::Vec3& gearoenix::physics::Transformation::get_location() const noexcept
{
    GXUNIMPLEMENTED;
}

void gearoenix::physics::Transformation::set_location(const math::Vec3&) noexcept
{
    GXUNIMPLEMENTED;
}

void gearoenix::physics::Transformation::translate(const math::Vec3&) noexcept
{
    GXUNIMPLEMENTED;
}

void gearoenix::physics::Transformation::local_x_translate(const core::Real) noexcept
{
    GXUNIMPLEMENTED;
}

void gearoenix::physics::Transformation::local_y_translate(const core::Real) noexcept
{
    GXUNIMPLEMENTED;
}

void gearoenix::physics::Transformation::local_z_translate(const core::Real) noexcept
{
    GXUNIMPLEMENTED;
}

void gearoenix::physics::Transformation::global_rotate(const core::Real, const math::Vec3&, const math::Vec3&) noexcept
{
    GXUNIMPLEMENTED;
}

void gearoenix::physics::Transformation::global_rotate(const core::Real, const math::Vec3&) noexcept
{
    GXUNIMPLEMENTED;
}

void gearoenix::physics::Transformation::local_rotate(const core::Real, const math::Vec3&) noexcept
{
    GXUNIMPLEMENTED;
}

void gearoenix::physics::Transformation::local_x_rotate(const core::Real) noexcept
{
    GXUNIMPLEMENTED;
}

void gearoenix::physics::Transformation::local_y_rotate(const core::Real) noexcept
{
    GXUNIMPLEMENTED;
}

void gearoenix::physics::Transformation::local_z_rotate(const core::Real) noexcept
{
    GXUNIMPLEMENTED;
}

void gearoenix::physics::Transformation::scale(const core::Real) noexcept
{
    GXUNIMPLEMENTED;
}

void gearoenix::physics::Transformation::set_orientation(const math::Quat&) noexcept
{
    GXUNIMPLEMENTED;
}
