#include "rnd-cmr-perspective.hpp"
#include "../../core/event/cr-ev-event.hpp"
#include "../../system/sys-file.hpp"
#include <cmath>

void gearoenix::render::camera::Perspective::on_ratio_change()
{
    c_height = c_width / screen_ratio;
    tanvang = c_height / start;
    one_cosvang = 1.0f / std::cos(std::atan(tanhang));
    p = math::Mat4x4::perspective(c_width * 2.0f, c_height * 2.0f, start, end);
    vp = p * v;
}

gearoenix::render::camera::Perspective::Perspective(system::File* f, system::Application* app)
    : Camera(f, app)
{
    f->read(h_angle);
    tanhang = std::tan(h_angle);
    one_coshang = 1.0f / std::cos(h_angle);
    c_width = tanhang * start;
    on_ratio_change();
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
    if (eye_on_x - (radius * one_coshang) > tanhang * eye_z)
        return false;
    core::Real eye_on_y = std::abs(eye_on_z_plane.dot(y));
    if (eye_on_y - (radius * one_cosvang) > tanvang * eye_z)
        return false;

    return true;
}

void gearoenix::render::camera::Perspective::on_event(const core::event::Event& e)
{
    Camera::on_event(e);
    const core::event::WindowResize* event = e.to_window_resize();
    if (nullptr != event) {
        on_ratio_change();
    }
}
