#include "rnd-cmr-orthographic.hpp"
#include "../../core/event/cr-ev-event.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include <cmath>

void gearoenix::render::camera::Orthographic::on_ratio_change()
{
//    c_width = aspects_size * screen_ratio;
//    p = math::Mat4x4::orthographic(c_width * 2.0f, c_height * 2.0f, start, end);
//    vp = p * v;
}

gearoenix::render::camera::Orthographic::Orthographic(
        const core::Id my_id,
        const std::shared_ptr<system::stream::Stream> &f,
        const std::shared_ptr<engine::Engine> &e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore> &c)
    : Camera(my_id, f, e, c)
{
//    f->read(aspects_size);
//    c_height = aspects_size;
//    on_ratio_change();
}

bool gearoenix::render::camera::Orthographic::in_sight(const math::Vec3& location, const core::Real radius) const
{
//    math::Vec3 eye = location - l;
//    core::Real eye_z = -(eye.dot(z_axis));
//    if (eye_z < 0.0f)
//        return eye.square_length() < (radius * radius);
//    if (eye_z - radius > end)
//        return false;
//    math::Vec3 eye_on_z_plane = eye - (z_axis * (-eye_z));
//    core::Real eye_on_x = std::abs(eye_on_z_plane.dot(x_axis));
//    if (eye_on_x - radius > c_width)
//        return false;
//    core::Real eye_on_y = std::abs(eye_on_z_plane.dot(y_axis));
//    if (eye_on_y - radius > c_height)
//        return false;
//    return true;
}

void gearoenix::render::camera::Orthographic::on_event(const core::event::Event& e)
{
    Camera::on_event(e);
    if (e.get_type() == core::event::Event::From::WINDOW_RESIZE)
        on_ratio_change();
}

gearoenix::math::Ray3 gearoenix::render::camera::Orthographic::create_ray3(const core::Real x, const core::Real y) const
{
//    math::Vec3 dir = -z_axis;
//    math::Vec3 origin = (x_axis * x) + (y_axis * y) + (dir * start) + l;
//    return math::Ray3(origin, dir);
}

gearoenix::core::Real gearoenix::render::camera::Orthographic::get_distance(const math::Vec3 &model_location) const
{
//    return -((model_location - l).dot(z_axis));
}
