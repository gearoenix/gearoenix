#include "rnd-cmr-perspective.hpp"
#include "../../system/sys-file.hpp"
#include <cmath>

gearoenix::render::camera::Perspective::Perspective(system::File* f, system::Application* app)
    : Camera(f, app)
{
    f->read(h_angle);
    c_width = std::tan(h_angle) * start * 2.0f;
    c_height = c_width / screen_ratio;
    v_angle = std::atan(c_height / start);
    p = math::Mat4x4::perspective(c_width, c_height, start, end);
    vp = p * v;
    tanhang = std::tan(h_angle);
    sinhang = std::sin(h_angle);
    icoshang = 1.0f / std::cos(h_angle);
    tanvang = std::tan(v_angle);
    sinvang = std::sin(v_angle);
    icosvang = 1.0f / std::cos(v_angle);
}

bool gearoenix::render::camera::Perspective::in_sight(const math::Vec3& location, const core::Real radius) const
{
    math::Vec3 eye = location - l;
    core::Real eye_z = -(z.dot(eye));
    if (eye_z < 0.0f)
        return eye.square_length() < (radius * radius);
    if (eye_z - radius > end)
        return false;
    core::Real eye_len = eye.length();
    math::Vec3 eye_on_z_plane = eye - (z * (-eye_z));
    core::Real eye_on_x = std::abs(eye_on_z_plane.dot(x));
    if (((eye_on_x - radius) / eye_z) > tanhang)
        return false;
    core::Real eye_on_y = std::abs(eye_on_z_plane.dot(y));
    if (((eye_on_y - radius) / eye_z) > tanvang)
        return false;

    return true;

    core::Real dis = eye.square_length();
    core::Real ang = std::acos(-(z.dot(eye)));

    math::Vec3 el = location - l;
    core::Real ezl = -z.dot(el);
    math::Vec3 ez = z * ezl;
    math::Vec3 lez = el - ez;
    core::Real wel = lez.dot(x);
    core::Real wcz = wel * tanhang;
    core::Real ezpl = wcz + ezl;
    core::Real ws = sinhang * ezpl;
    core::Real w = icoshang * wel;
    if (w > ws + radius)
        return false;
    wel = lez.dot(y);
    wcz = wel * tanvang;
    ezpl = wcz + ezl;
    ws = sinvang * ezpl;
    w = icosvang * wel;
    return w < ws + radius;
}

void gearoenix::render::camera::Perspective::window_size_changed()
{
}
