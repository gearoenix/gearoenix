#include "phs-transformation.hpp"
#include "../math/math-quaternion.hpp"
#include "../system/sys-log.hpp"

gearoenix::physics::Transformation::Transformation()
{
}

gearoenix::physics::Transformation::~Transformation()
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

const gearoenix::math::Mat4x4& gearoenix::physics::Transformation::get_transformation() const noexcept
{
	GXUNIMPLEMENTED;
}

void gearoenix::physics::Transformation::get_location(math::Vec3& l) const noexcept
{
	GXUNIMPLEMENTED;
}

const gearoenix::math::Vec3 gearoenix::physics::Transformation::get_location() const noexcept
{
	GXUNIMPLEMENTED;
}

void gearoenix::physics::Transformation::set_location(const math::Vec3& l) noexcept
{
	GXUNIMPLEMENTED;
}

void gearoenix::physics::Transformation::translate(const math::Vec3& t) noexcept
{
	GXUNIMPLEMENTED;
}

void gearoenix::physics::Transformation::local_x_translate(const core::Real t) noexcept
{
	GXUNIMPLEMENTED;
}

void gearoenix::physics::Transformation::local_y_translate(const core::Real t) noexcept
{
	GXUNIMPLEMENTED;
}

void gearoenix::physics::Transformation::local_z_translate(const core::Real t) noexcept
{
	GXUNIMPLEMENTED;
}

void gearoenix::physics::Transformation::global_rotate(const core::Real d, const math::Vec3& axis, const math::Vec3& location) noexcept
{
	GXUNIMPLEMENTED;
}

void gearoenix::physics::Transformation::global_rotate(const core::Real d, const math::Vec3& axis) noexcept
{
	GXUNIMPLEMENTED;
}

void gearoenix::physics::Transformation::local_rotate(const core::Real d, const math::Vec3& axis) noexcept
{
	GXUNIMPLEMENTED;
}

void gearoenix::physics::Transformation::local_x_rotate(const core::Real d) noexcept
{
	GXUNIMPLEMENTED;
}

void gearoenix::physics::Transformation::local_y_rotate(const core::Real d) noexcept
{
	GXUNIMPLEMENTED;
}

void gearoenix::physics::Transformation::local_z_rotate(const core::Real d) noexcept
{
	GXUNIMPLEMENTED;
}

void gearoenix::physics::Transformation::scale(const core::Real s) noexcept
{
	GXUNIMPLEMENTED;
}

void gearoenix::physics::Transformation::set_orientation(const math::Quat& q) noexcept
{
	GXUNIMPLEMENTED;
}
