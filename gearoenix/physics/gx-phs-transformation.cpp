#include "gx-phs-transformation.hpp"

gearoenix::physics::Transformation::Transformation() noexcept
    : core::ecs::Component(this)
    , x_axis(1.0, 0.0, 0.0)
    , y_axis(0.0, 1.0, 0.0)
    , z_axis(0.0, 0.0, 1.0)
    , scale(1.0)
{
}

gearoenix::math::Vec3<double> gearoenix::physics::Transformation::get_location() const noexcept
{
    return matrix.get_location();
}

void gearoenix::physics::Transformation::get_location(math::Vec3<double>& l) const noexcept
{
    matrix.get_location(l);
}

void gearoenix::physics::Transformation::set_location(const math::Vec3<double>& l) noexcept
{
    matrix.set_location(l);
}

void gearoenix::physics::Transformation::translate(const math::Vec3<double>& t) noexcept
{
    matrix.translate(t);
}

void gearoenix::physics::Transformation::local_x_translate(const double v) noexcept
{
    translate(x_axis * v);
}

void gearoenix::physics::Transformation::local_y_translate(const double v) noexcept
{
    translate(y_axis * v);
}

void gearoenix::physics::Transformation::local_z_translate(const double v) noexcept
{
    translate(z_axis * v);
}

void gearoenix::physics::Transformation::global_rotate(const double d, const math::Vec3<double>& axis, const math::Vec3<double>& l) noexcept
{
    const auto loc = get_location();
    translate(math::Vec3(0.0));
    const auto r = decltype(matrix)::rotation(axis, d);
    x_axis = (r * math::Vec4(x_axis, 0.0)).xyz();
    y_axis = (r * math::Vec4(y_axis, 0.0)).xyz();
    z_axis = (r * math::Vec4(z_axis, 0.0)).xyz();
    matrix = r * matrix;
    translate((r * math::Vec4(l - loc, 0.0)).xyz() + loc);
}

void gearoenix::physics::Transformation::global_rotate(const double d, const math::Vec3<double>& axis) noexcept
{
    const auto r = decltype(matrix)::rotation(axis, d);
    x_axis = (r * math::Vec4(x_axis, 0.0)).xyz();
    y_axis = (r * math::Vec4(y_axis, 0.0)).xyz();
    z_axis = (r * math::Vec4(z_axis, 0.0)).xyz();
    matrix = r * matrix;
}

void gearoenix::physics::Transformation::local_rotate(const double d, const math::Vec3<double>& a) noexcept
{
    const auto loc = get_location();
    translate(math::Vec3(0.0));
    global_rotate(d, a);
    translate(loc);
}

void gearoenix::physics::Transformation::local_x_rotate(const double d) noexcept
{
    local_rotate(d, x_axis);
}

void gearoenix::physics::Transformation::local_y_rotate(const double d) noexcept
{
    local_rotate(d, y_axis);
}

void gearoenix::physics::Transformation::local_z_rotate(const double d) noexcept
{
    local_rotate(d, z_axis);
}

void gearoenix::physics::Transformation::local_scale(const double s) noexcept
{
    matrix.local_scale(s);
    scale *= s;
}

void gearoenix::physics::Transformation::local_scale(const math::Vec3<double>& s) noexcept
{
    matrix.local_scale(s);
    scale *= s;
}

void gearoenix::physics::Transformation::local_x_scale(const double s) noexcept
{
    matrix.local_x_scale(s);
    scale.x *= s;
}

void gearoenix::physics::Transformation::local_y_scale(const double s) noexcept
{
    matrix.local_y_scale(s);
    scale.y *= s;
}

void gearoenix::physics::Transformation::local_z_scale(const double s) noexcept
{
    matrix.local_z_scale(s);
    scale.z *= s;
}

void gearoenix::physics::Transformation::set_orientation(const math::Quat<double>& q) noexcept
{
    const auto r = q.to_mat();
    const auto l = get_location();
    translate(math::Vec3(0.0));
    x_axis = (r * math::Vec4(x_axis, 0.0)).xyz();
    y_axis = (r * math::Vec4(y_axis, 0.0)).xyz();
    z_axis = (r * math::Vec4(z_axis, 0.0)).xyz();
    matrix = r * matrix;
    translate(l);
}
