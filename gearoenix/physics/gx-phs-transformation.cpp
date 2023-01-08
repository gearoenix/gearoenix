#include "gx-phs-transformation.hpp"
#include <imgui/imgui.h>

gearoenix::physics::Transformation::Transformation(std::string&& name) noexcept
    : core::ecs::Component(this, std::move(name))
    , x_axis(1.0, 0.0, 0.0)
    , y_axis(0.0, 1.0, 0.0)
    , z_axis(0.0, 0.0, 1.0)
    , scale(1.0)
{
}

gearoenix::math::Vec3<double> gearoenix::physics::Transformation::get_global_location() const noexcept
{
    return global_matrix.get_location();
}

void gearoenix::physics::Transformation::get_global_location(math::Vec3<double>& l) const noexcept
{
    global_matrix.get_location(l);
}

gearoenix::math::Vec3<double> gearoenix::physics::Transformation::get_local_location() const noexcept
{
    return local_matrix.get_location();
}

void gearoenix::physics::Transformation::get_local_location(math::Vec3<double>& l) const noexcept
{
    local_matrix.get_location(l);
}

void gearoenix::physics::Transformation::set_local_location(const math::Vec3<double>& l) noexcept
{
    changed = true;
    local_matrix.set_location(l);
}

void gearoenix::physics::Transformation::local_translate(const math::Vec3<double>& t) noexcept
{
    changed = true;
    local_matrix.translate(t);
}

void gearoenix::physics::Transformation::local_x_translate(const double v) noexcept
{
    changed = true;
    local_translate(x_axis * v);
}

void gearoenix::physics::Transformation::local_y_translate(const double v) noexcept
{
    changed = true;
    local_translate(y_axis * v);
}

void gearoenix::physics::Transformation::local_z_translate(const double v) noexcept
{
    changed = true;
    local_translate(z_axis * v);
}

void gearoenix::physics::Transformation::local_outer_rotate(const double d, const math::Vec3<double>& axis, const math::Vec3<double>& l) noexcept
{
    changed = true;
    const auto loc = get_local_location();
    local_translate(math::Origin3D<double>);
    const auto r = decltype(local_matrix)::rotation(axis, d);
    x_axis = (r * math::Vec4(x_axis, 0.0)).xyz();
    y_axis = (r * math::Vec4(y_axis, 0.0)).xyz();
    z_axis = (r * math::Vec4(z_axis, 0.0)).xyz();
    local_matrix = r * local_matrix;
    local_translate((r * math::Vec4(l - loc, 0.0)).xyz() + loc);
}

void gearoenix::physics::Transformation::local_outer_rotate(const double d, const math::Vec3<double>& axis) noexcept
{
    changed = true;
    const auto r = decltype(local_matrix)::rotation(axis, d);
    x_axis = (r * math::Vec4(x_axis, 0.0)).xyz();
    y_axis = (r * math::Vec4(y_axis, 0.0)).xyz();
    z_axis = (r * math::Vec4(z_axis, 0.0)).xyz();
    local_matrix = r * local_matrix;
}

void gearoenix::physics::Transformation::local_rotate(const double d, const math::Vec3<double>& axis) noexcept
{
    changed = true;
    const auto loc = local_matrix.get_location();
    local_matrix.set_location(math::Origin3D<double>);
    const auto r = decltype(local_matrix)::rotation(axis, d);
    x_axis = (r * math::Vec4(x_axis, 0.0)).xyz();
    y_axis = (r * math::Vec4(y_axis, 0.0)).xyz();
    z_axis = (r * math::Vec4(z_axis, 0.0)).xyz();
    local_matrix = r * local_matrix;
    local_matrix.set_location(loc);
}

void gearoenix::physics::Transformation::local_rotate(const math::Quat<double>& q) noexcept
{
    changed = true;
    const auto loc = local_matrix.get_location();
    local_matrix.set_location(math::Origin3D<double>);
    const auto r = q.to_mat();
    x_axis = (r * math::Vec4(x_axis, 0.0)).xyz();
    y_axis = (r * math::Vec4(y_axis, 0.0)).xyz();
    z_axis = (r * math::Vec4(z_axis, 0.0)).xyz();
    local_matrix = r * local_matrix;
    local_matrix.set_location(loc);
}

void gearoenix::physics::Transformation::local_x_rotate(const double d) noexcept
{
    changed = true;
    const auto loc = local_matrix.get_location();
    local_matrix.set_location(math::Origin3D<double>);
    const auto r = decltype(local_matrix)::rotation(x_axis, d);
    y_axis = (r * math::Vec4(y_axis, 0.0)).xyz();
    z_axis = (r * math::Vec4(z_axis, 0.0)).xyz();
    local_matrix = r * local_matrix;
    local_matrix.set_location(loc);
}

void gearoenix::physics::Transformation::local_y_rotate(const double d) noexcept
{
    changed = true;
    const auto loc = local_matrix.get_location();
    local_matrix.set_location(math::Origin3D<double>);
    const auto r = decltype(local_matrix)::rotation(y_axis, d);
    x_axis = (r * math::Vec4(x_axis, 0.0)).xyz();
    z_axis = (r * math::Vec4(z_axis, 0.0)).xyz();
    local_matrix = r * local_matrix;
    local_matrix.set_location(loc);
}

void gearoenix::physics::Transformation::local_z_rotate(const double d) noexcept
{
    changed = true;
    const auto loc = local_matrix.get_location();
    local_matrix.set_location(math::Origin3D<double>);
    const auto r = decltype(local_matrix)::rotation(z_axis, d);
    x_axis = (r * math::Vec4(x_axis, 0.0)).xyz();
    y_axis = (r * math::Vec4(y_axis, 0.0)).xyz();
    local_matrix = r * local_matrix;
    local_matrix.set_location(loc);
}

void gearoenix::physics::Transformation::local_scale(const double s) noexcept
{
    changed = true;
    local_matrix.local_scale(s);
    scale *= s;
}

void gearoenix::physics::Transformation::local_scale(const math::Vec3<double>& s) noexcept
{
    changed = true;
    local_matrix.local_scale(s);
    scale *= s;
}

void gearoenix::physics::Transformation::local_x_scale(const double s) noexcept
{
    changed = true;
    local_matrix.local_x_scale(s);
    scale.x *= s;
}

void gearoenix::physics::Transformation::local_y_scale(const double s) noexcept
{
    changed = true;
    local_matrix.local_y_scale(s);
    scale.y *= s;
}

void gearoenix::physics::Transformation::local_z_scale(const double s) noexcept
{
    changed = true;
    local_matrix.local_z_scale(s);
    scale.z *= s;
}

void gearoenix::physics::Transformation::set_local_orientation(const math::Quat<double>& q) noexcept
{
    changed = true;
    const auto r = q.to_mat();
    const auto l = get_local_location();
    local_translate(math::Vec3(0.0));
    x_axis = (r * math::Vec4(x_axis, 0.0)).xyz();
    y_axis = (r * math::Vec4(y_axis, 0.0)).xyz();
    z_axis = (r * math::Vec4(z_axis, 0.0)).xyz();
    local_matrix = r * local_matrix;
    local_translate(l);
}

gearoenix::math::Quat<double> gearoenix::physics::Transformation::get_local_orientation() const noexcept
{
    const double trace = x_axis.x + y_axis.y + z_axis.z;
    if (trace > 0.0) {
        const double s = 0.5 / sqrt(trace + 1.0);
        return {
            (y_axis.z - z_axis.y) * s,
            (z_axis.x - x_axis.z) * s,
            (x_axis.y - y_axis.x) * s,
            0.25 / s
        };
    }
    if (x_axis.x > y_axis.y && x_axis.x > z_axis.z) {
        const double s = 0.5 / sqrt(1.0 + x_axis.x - y_axis.y - z_axis.z);
        return {
            0.25 / s,
            (y_axis.x + x_axis.y) * s,
            (z_axis.x + x_axis.z) * s,
            (y_axis.z - z_axis.y) * s,
        };
    }
    if (y_axis.y > z_axis.z) {
        const double s = 0.5 / sqrt(1.0 + y_axis.y - x_axis.x - z_axis.z);
        return {
            (y_axis.x + x_axis.y) * s,
            0.25 / s,
            (z_axis.y + y_axis.z) * s,
            (z_axis.x - x_axis.z) * s
        };
    }
    const double s = 0.5 / sqrt(1.0 + z_axis.z - x_axis.x - y_axis.y);
    return {
        (z_axis.x + x_axis.z) * s,
        (z_axis.y + y_axis.z) * s,
        0.25 / s,
        (x_axis.y - y_axis.x) * s
    };
}

void gearoenix::physics::Transformation::local_look_at(const math::Vec3<double>& location, const math::Vec3<double>& target, const math::Vec3<double>& up) noexcept
{
    changed = true;
    z_axis = (location - target).normalised();
    x_axis = up.cross(z_axis).normalised();
    y_axis = z_axis.cross(x_axis).normalised();
    local_matrix = math::Mat4x4<double>::look_at(location, -x_axis, y_axis, -z_axis).inverted();
    scale = math::Vec3<double>(1.0);
}

void gearoenix::physics::Transformation::local_look_at(const math::Vec3<double>& target, const math::Vec3<double>& up) noexcept
{
    changed = true;
    math::Vec3<double> l;
    get_local_location(l);
    z_axis = (l - target).normalised();
    x_axis = up.cross(z_axis).normalised();
    y_axis = z_axis.cross(x_axis).normalised();
    local_matrix = math::Mat4x4<double>::look_at(l, -x_axis, y_axis, -z_axis).inverted();
    scale = math::Vec3<double>(1.0);
}

void gearoenix::physics::Transformation::local_update() noexcept
{
    if (changed) {
        global_matrix = local_matrix;
        inverted_global_matrix = global_matrix.inverted();
    }
}

void gearoenix::physics::Transformation::update(const Transformation& parent) noexcept
{
    if (parent.changed)
        changed = true;
    if (changed) {
        global_matrix = parent.global_matrix * local_matrix;
        inverted_global_matrix = global_matrix.inverted();
    }
}

void gearoenix::physics::Transformation::local_update_without_inverse() noexcept
{
    if (changed) {
        global_matrix = local_matrix;
    }
}

void gearoenix::physics::Transformation::update_without_inverse(const Transformation& parent) noexcept
{
    if (parent.changed)
        changed = true;
    if (changed) {
        global_matrix = parent.global_matrix * local_matrix;
    }
}

void gearoenix::physics::Transformation::clear_change() noexcept
{
    changed = false;
}

void gearoenix::physics::Transformation::reset() noexcept
{
    global_matrix = local_matrix = inverted_global_matrix = math::Mat4x4<double>();
    x_axis = math::X3D<double>;
    y_axis = math::Y3D<double>;
    z_axis = math::Z3D<double>;
    scale = math::Vec3(1.0);
    changed = false;
}

void gearoenix::physics::Transformation::reset(
    const math::Vec3<double>& s,
    const math::Quat<double>& r,
    const math::Vec3<double>& l) noexcept
{
    local_matrix = r.to_mat();
    x_axis = { local_matrix.data[0][0], local_matrix.data[0][1], local_matrix.data[0][2] };
    y_axis = { local_matrix.data[1][0], local_matrix.data[1][1], local_matrix.data[1][2] };
    z_axis = { local_matrix.data[2][0], local_matrix.data[2][1], local_matrix.data[2][2] };
    scale = s;
    local_matrix.local_scale(s);
    local_matrix.set_location(l);
    changed = true;
}

void gearoenix::physics::Transformation::show_gui() noexcept
{
    if (ImGui::TreeNode("Transformation")) {
        auto l = get_local_location();
        bool input_changed = false;
        if (ImGui::TreeNode("Position")) {
            input_changed |= ImGui::InputDouble("x", &l.x, 0.01, 1.0, "%.3f");
            input_changed |= ImGui::InputDouble("y", &l.y, 0.01, 1.0, "%.3f");
            input_changed |= ImGui::InputDouble("z", &l.z, 0.01, 1.0, "%.3f");
            ImGui::TreePop();
        }
        if (input_changed)
            set_local_location(l);
        ImGui::TreePop();
    }
}

void gearoenix::physics::Transformation::reset(
    const math::Vec3<double>& s,
    const math::Vec3<double>& x,
    const math::Vec3<double>& y,
    const math::Vec3<double>& z,
    const math::Vec3<double>& l) noexcept
{
    x_axis = x;
    y_axis = y;
    z_axis = z;
    local_matrix.data[0][0] = x.x;
    local_matrix.data[0][1] = x.y;
    local_matrix.data[0][2] = x.z;
    local_matrix.data[0][3] = 0.0;
    local_matrix.data[1][0] = y.x;
    local_matrix.data[1][1] = y.y;
    local_matrix.data[1][2] = y.z;
    local_matrix.data[1][3] = 0.0;
    local_matrix.data[2][0] = z.x;
    local_matrix.data[2][1] = z.y;
    local_matrix.data[2][2] = z.z;
    local_matrix.data[2][3] = 0.0;
    local_matrix.data[3][0] = 0.0;
    local_matrix.data[3][1] = 0.0;
    local_matrix.data[3][2] = 0.0;
    local_matrix.data[3][3] = 1.0;
    scale = s;
    local_matrix.local_scale(s);
    local_matrix.set_location(l);
    changed = true;
}
