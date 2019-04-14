#include "phs-transformation.hpp"
#include "../math/math-quaternion.hpp"

gearoenix::physics::Transformation::Transformation()
    : x_axis(1.0f, 0.0f, 0.0f)
    , y_axis(0.0f, 1.0f, 0.0f)
    , z_axis(0.0f, 0.0f, 1.0f)
{
}

gearoenix::physics::Transformation::~Transformation()
{
}

const gearoenix::math::Vec3& gearoenix::physics::Transformation::get_x_axis() const
{
    return x_axis;
}

const gearoenix::math::Vec3& gearoenix::physics::Transformation::get_y_axis() const
{
    return y_axis;
}

const gearoenix::math::Vec3& gearoenix::physics::Transformation::get_z_axis() const
{
    return z_axis;
}

const gearoenix::math::Mat4x4& gearoenix::physics::Transformation::get_transformation() const
{
    return transformation;
}

void gearoenix::physics::Transformation::get_location(math::Vec3& l) const
{
    transformation.get_location(l);
}

const gearoenix::math::Vec3 gearoenix::physics::Transformation::get_location() const
{
    math::Vec3 l;
    get_location(l);
    return l;
}

void gearoenix::physics::Transformation::set_location(const math::Vec3& l)
{
    transformation.set_location(l);
}

void gearoenix::physics::Transformation::translate(const math::Vec3& t)
{
    transformation.translate(t);
}

void gearoenix::physics::Transformation::local_x_translate(const core::Real t)
{
    translate(x_axis * t);
}

void gearoenix::physics::Transformation::local_y_translate(const core::Real t)
{
    translate(y_axis * t);
}

void gearoenix::physics::Transformation::local_z_translate(const core::Real t)
{
    translate(z_axis * t);
}

void gearoenix::physics::Transformation::global_rotate(const core::Real d, const math::Vec3& axis, const math::Vec3& location)
{
    translate(-location);
    global_rotate(d, axis);
    translate(location);
}

void gearoenix::physics::Transformation::global_rotate(const core::Real d, const math::Vec3& axis)
{
    const math::Mat4x4 r = math::Mat4x4::rotation(axis, d);
    x_axis = r * x_axis;
    y_axis = r * y_axis;
    z_axis = r * z_axis;
    transformation = r * transformation;
}

void gearoenix::physics::Transformation::local_rotate(const core::Real d, const math::Vec3& axis)
{
    math::Vec3 l;
    transformation.get_location(l);
    transformation.set_location_zero();
    const math::Mat4x4 r = math::Mat4x4::rotation(axis, d);
    x_axis = r * x_axis;
    y_axis = r * y_axis;
    z_axis = r * z_axis;
    transformation = r * transformation;
    transformation.set_location(l);
}

void gearoenix::physics::Transformation::local_x_rotate(const core::Real d)
{
    local_rotate(d, x_axis);
}

void gearoenix::physics::Transformation::local_y_rotate(const core::Real d)
{
    local_rotate(d, y_axis);
}

void gearoenix::physics::Transformation::local_z_rotate(const core::Real d)
{
    local_rotate(d, z_axis);
}

void gearoenix::physics::Transformation::global_scale(const core::Real s)
{
    transformation.global_scale(s, s, s, s);
}

void gearoenix::physics::Transformation::local_scale(const core::Real s)
{
    transformation.local_scale(s);
}

void gearoenix::physics::Transformation::set_orientation(const math::Quat& q)
{
    math::Vec3 l;
    transformation.get_location(l);
    transformation.set_location_zero();
    const math::Mat4x4 r = q.to_mat();
    transformation = r;
    x_axis = l * x_axis;
    y_axis = l * y_axis;
    z_axis = l * z_axis;
    transformation.set_location(l);
}
