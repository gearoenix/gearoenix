#include "rnd-cmr-perspective.hpp"
#include "../../core/event/cr-ev-event.hpp"
#include "../../core/event/cr-ev-window-resize.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-log.hpp"
#include <cmath>

void gearoenix::render::camera::Perspective::on_ratio_change()
{
    //    c_height = c_width / screen_ratio;
    //    tanvang = c_height / start;
    //    one_cosvang = 1.0f / std::cos(std::atan(tanhang));
    //    p = math::Mat4x4::perspective(c_width * 2.0f, c_height * 2.0f, start, end);
    //    vp = p * v;
}

gearoenix::render::camera::Perspective::Perspective(
    const core::Id my_id,
    const std::shared_ptr<system::stream::Stream>& f,
    const std::shared_ptr<engine::Engine>& e)
    : Camera(my_id, f, e)
{
    const core::Real rad = f->read<core::Real>();
    GXLOGD("Radiant is: " << rad << ", in perspective camera with id: " << my_id);
    set_field_of_view(rad);
}

bool gearoenix::render::camera::Perspective::in_sight(const math::Vec3& location, const core::Real radius) const
{
    //    math::Vec3 eye = location - l;
    //    core::Real eye_z = -(z_axis.dot(eye));
    //    if (eye_z < 0.0f)
    //        return eye.square_length() < (radius * radius);
    //    if (eye_z - radius > end)
    //        return false;
    //    math::Vec3 eye_on_z_plane = eye - (z_axis * (-eye_z));
    //    core::Real eye_on_x = std::abs(eye_on_z_plane.dot(x_axis));
    //    if (eye_on_x - (radius * one_coshang) > tanhang * eye_z)
    //        return false;
    //    core::Real eye_on_y = std::abs(eye_on_z_plane.dot(y_axis));
    //    if (eye_on_y - (radius * one_cosvang) > tanvang * eye_z)
    //        return false;
    return true;
}

void gearoenix::render::camera::Perspective::on_event(const core::event::Event& e)
{
    Camera::on_event(e);
    if (e.get_type() == core::event::Event::From::WINDOW_RESIZE)
        on_ratio_change();
}

gearoenix::math::Ray3 gearoenix::render::camera::Perspective::create_ray3(const core::Real x, const core::Real y) const
{
    math::Vec3 dir = (uniform.x_reserved.xyz() * x) + (uniform.y_reserved.xyz() * y) + (uniform.z_reserved.xyz() * uniform.near_aspect_ratio_reserved[0]);
    const math::Vec3 origin = dir + uniform.position_far.xyz();
    dir.normalize();
    return math::Ray3(origin, dir);
}

gearoenix::core::Real gearoenix::render::camera::Perspective::get_distance(const math::Vec3 model_location) const
{
    return (model_location - uniform.position_far.xyz()).square_length();
}

void gearoenix::render::camera::Perspective::set_field_of_view(const core::Real radian)
{
    fovx = radian;
    tanx = static_cast<core::Real>(std::tan(static_cast<double>(radian * 0.5f)));
    tany = tanx / uniform.near_aspect_ratio_reserved[1];
    fovy = static_cast<core::Real>(std::atan(static_cast<double>(tany))) * 2.0f;
    uniform.projection = math::Mat4x4::perspective(
        tanx * -uniform.near_aspect_ratio_reserved[0] * 2.0f,
        tany * -uniform.near_aspect_ratio_reserved[0] * 2.0f,
        -uniform.near_aspect_ratio_reserved[0],
        -uniform.position_far[3]);
    uniform.uniform_projection = math::Mat4x4(
                                     0.5f, 0.0f, 0.0f, 0.0f,
                                     0.0f, 0.5f, 0.0f, 0.0f,
                                     0.0f, 0.0f, 1.0f, 0.0f,
                                     0.5f, 0.5f, 0.0f, 1.0f)
        * uniform.projection;
    lambda = static_cast<core::Real>(
                 std::sin(static_cast<double>(fovx * 0.5f)) + std::sin(static_cast<double>(fovy * 0.5f)))
        * 0.5f;
    update_view_projection();
}
