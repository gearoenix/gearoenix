#include "phs-transformable.hpp"
#include "../math/math-matrix.hpp"

void gearoenix::physics::Transferable::local_x_rotate(const core::Real d, math::Mat4x4& rm)
{
    rm = math::Mat4x4::rotation(x_axis, d);
    y_axis = rm * y_axis;
    z_axis = rm * z_axis;
}

void gearoenix::physics::Transferable::local_y_rotate(const core::Real d, math::Mat4x4& rm)
{
    rm = math::Mat4x4::rotation(y_axis, d);
    x_axis = rm * x_axis;
    z_axis = rm * z_axis;
}

void gearoenix::physics::Transferable::local_z_rotate(const core::Real d, math::Mat4x4& rm)
{
    rm = math::Mat4x4::rotation(z_axis, d);
    x_axis = rm * x_axis;
    y_axis = rm * y_axis;
}

void gearoenix::physics::Transferable::local_rotate(const core::Real d, const math::Vec3& a, math::Mat4x4& rm)
{
    rm = math::Mat4x4::rotation(a, d);
    x_axis = rm * x_axis;
    y_axis = rm * y_axis;
    z_axis = rm * z_axis;
}

void gearoenix::physics::Transferable::initialize_axis(const math::Mat4x4& t)
{
    math::Vec3 l;
    t.get_location(l);
    x_axis = (t * x_axis) - l;
    y_axis = (t * y_axis) - l;
    z_axis = (t * z_axis) - l;
    x_axis.normalize();
    y_axis.normalize();
    z_axis.normalize();
}

gearoenix::physics::Transferable::Transferable()
    : x_axis(1.0f, 0.0f, 0.0f)
    , y_axis(0.0f, 1.0f, 0.0f)
    , z_axis(0.0f, 0.0f, 1.0f)
{
}

gearoenix::physics::Transferable::~Transferable()
{
}

const gearoenix::math::Vec3& gearoenix::physics::Transferable::get_x_axis() const
{
    return x_axis;
}

const gearoenix::math::Vec3& gearoenix::physics::Transferable::get_y_axis() const
{
    return y_axis;
}

const gearoenix::math::Vec3& gearoenix::physics::Transferable::get_z_axis() const
{
    return z_axis;
}

bool gearoenix::physics::Transferable::is_transformed() const
{
    return transformed;
}

void gearoenix::physics::Transferable::commit_transformation()
{
    transformed = false;
}

void gearoenix::physics::Transferable::local_x_translate(const core::Real t)
{
    translate(x_axis * t);
}

void gearoenix::physics::Transferable::local_y_translate(const core::Real t)
{
    translate(y_axis * t);
}

void gearoenix::physics::Transferable::local_z_translate(const core::Real t)
{
    translate(z_axis * t);
}

void gearoenix::physics::Transferable::global_rotate(const core::Real d, const math::Vec3& axis, const math::Vec3& location)
{
    translate(-location);
    global_rotate(d, axis);
    translate(location);
}

void gearoenix::physics::Transferable::global_rotate(const math::Mat4x4& rm)
{
    x_axis = rm * x_axis;
    y_axis = rm * y_axis;
    z_axis = rm * z_axis;
}
