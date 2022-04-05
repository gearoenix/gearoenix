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
    changed = true;
    matrix.set_location(l);
}

void gearoenix::physics::Transformation::set_location(const double x, const double y, const double z) noexcept
{
    changed = true;
    matrix.set_location(math::Vec3(x, y, z));
}

void gearoenix::physics::Transformation::translate(const math::Vec3<double>& t) noexcept
{
    changed = true;
    matrix.translate(t);
}

void gearoenix::physics::Transformation::local_x_translate(const double v) noexcept
{
    changed = true;
    translate(x_axis * v);
}

void gearoenix::physics::Transformation::local_y_translate(const double v) noexcept
{
    changed = true;
    translate(y_axis * v);
}

void gearoenix::physics::Transformation::local_z_translate(const double v) noexcept
{
    changed = true;
    translate(z_axis * v);
}

void gearoenix::physics::Transformation::global_rotate(const double d, const math::Vec3<double>& axis, const math::Vec3<double>& l) noexcept
{
    changed = true;
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
    changed = true;
    const auto r = decltype(matrix)::rotation(axis, d);
    x_axis = (r * math::Vec4(x_axis, 0.0)).xyz();
    y_axis = (r * math::Vec4(y_axis, 0.0)).xyz();
    z_axis = (r * math::Vec4(z_axis, 0.0)).xyz();
    matrix = r * matrix;
}

void gearoenix::physics::Transformation::local_rotate(const double d, const math::Vec3<double>& axis) noexcept
{
    changed = true;
    const auto loc = matrix.get_location();
    matrix.set_location(math::Origin3D<double>);
    const auto r = decltype(matrix)::rotation(axis, d);
    x_axis = (r * math::Vec4(x_axis, 0.0)).xyz();
    y_axis = (r * math::Vec4(y_axis, 0.0)).xyz();
    z_axis = (r * math::Vec4(z_axis, 0.0)).xyz();
    matrix = r * matrix;
    matrix.set_location(loc);
}

void gearoenix::physics::Transformation::local_rotate_quaternion(const double x, const double y, const double z, const double w) noexcept
{
    changed = true;
    const auto loc = matrix.get_location();
    matrix.set_location(math::Origin3D<double>);
    math::Quat q(x, y, z, w);
    const auto r = q.to_mat();
    x_axis = (r * math::Vec4(x_axis, 0.0)).xyz();
    y_axis = (r * math::Vec4(y_axis, 0.0)).xyz();
    z_axis = (r * math::Vec4(z_axis, 0.0)).xyz();
    matrix = r * matrix;
    matrix.set_location(loc);
}

void gearoenix::physics::Transformation::local_x_rotate(const double d) noexcept
{
    changed = true;
    const auto loc = matrix.get_location();
    matrix.set_location(math::Origin3D<double>);
    const auto r = decltype(matrix)::rotation(x_axis, d);
    y_axis = (r * math::Vec4(y_axis, 0.0)).xyz();
    z_axis = (r * math::Vec4(z_axis, 0.0)).xyz();
    matrix = r * matrix;
    matrix.set_location(loc);
}

void gearoenix::physics::Transformation::local_y_rotate(const double d) noexcept
{
    changed = true;
    const auto loc = matrix.get_location();
    matrix.set_location(math::Origin3D<double>);
    const auto r = decltype(matrix)::rotation(y_axis, d);
    x_axis = (r * math::Vec4(x_axis, 0.0)).xyz();
    z_axis = (r * math::Vec4(z_axis, 0.0)).xyz();
    matrix = r * matrix;
    matrix.set_location(loc);
}

void gearoenix::physics::Transformation::local_z_rotate(const double d) noexcept
{
    changed = true;
    const auto loc = matrix.get_location();
    matrix.set_location(math::Origin3D<double>);
    const auto r = decltype(matrix)::rotation(z_axis, d);
    x_axis = (r * math::Vec4(x_axis, 0.0)).xyz();
    y_axis = (r * math::Vec4(y_axis, 0.0)).xyz();
    matrix = r * matrix;
    matrix.set_location(loc);
}

void gearoenix::physics::Transformation::local_scale(const double s) noexcept
{
    changed = true;
    matrix.local_scale(s);
    scale *= s;
}

void gearoenix::physics::Transformation::local_scale(const math::Vec3<double>& s) noexcept
{
    changed = true;
    matrix.local_scale(s);
    scale *= s;
}

void gearoenix::physics::Transformation::local_x_scale(const double s) noexcept
{
    changed = true;
    matrix.local_x_scale(s);
    scale.x *= s;
}

void gearoenix::physics::Transformation::local_y_scale(const double s) noexcept
{
    changed = true;
    matrix.local_y_scale(s);
    scale.y *= s;
}

void gearoenix::physics::Transformation::local_z_scale(const double s) noexcept
{
    changed = true;
    matrix.local_z_scale(s);
    scale.z *= s;
}

void gearoenix::physics::Transformation::set_orientation(const math::Quat<double>& q) noexcept
{
    changed = true;
    const auto r = q.to_mat();
    const auto l = get_location();
    translate(math::Vec3(0.0));
    x_axis = (r * math::Vec4(x_axis, 0.0)).xyz();
    y_axis = (r * math::Vec4(y_axis, 0.0)).xyz();
    z_axis = (r * math::Vec4(z_axis, 0.0)).xyz();
    matrix = r * matrix;
    translate(l);
}

void gearoenix::physics::Transformation::update() noexcept
{
    if (changed) {
        inverted_matrix = matrix.inverted();
    }
}

void gearoenix::physics::Transformation::clear() noexcept
{
    changed = false;
}
