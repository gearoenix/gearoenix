#include "rnd-cmr-perspective.hpp"
#include "../../system/sys-file.hpp"
#include <cmath>

gearoenix::render::camera::Perspective::Perspective(system::File* f, system::Application* app)
    : Camera(f, app)
{
    f->read(h_angle);
    c_width = std::tan(h_angle) * start;
    c_height = c_width / screen_ratio;
    tanvang = c_height / start;
    tanhang = c_width / start;
    p = math::Mat4x4::perspective(c_width * 2.0f, c_height * 2.0f, start, end);
    vp = p * v;
}

bool gearoenix::render::camera::Perspective::in_sight(const math::Vec3& location, const core::Real radius) const
{
    math::Vec3 eye = location - l;
    core::Real eye_z = -(z.dot(eye));
    if (eye_z < 0.0f)
        return eye.square_length() < (radius * radius);
    if (eye_z - radius > end)
        return false;
    math::Vec3 eye_on_z_plane = eye - (z * (-eye_z));
    core::Real eye_on_x = std::abs(eye_on_z_plane.dot(x));
    if (((eye_on_x - radius) / eye_z) > tanhang)
        return false;
    core::Real eye_on_y = std::abs(eye_on_z_plane.dot(y));
    if (((eye_on_y - radius) / eye_z) > tanvang)
        return false;

    return true;
}

void gearoenix::render::camera::Perspective::window_size_changed()
{
    UNIMPLEMENTED;
}
