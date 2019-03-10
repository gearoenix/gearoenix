#include "rnd-cmr-camera.hpp"
#include "../../core/event/cr-ev-event.hpp"
#include "../../core/event/cr-ev-window-resize.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-log.hpp"
#include "rnd-cmr-orthographic.hpp"
#include "rnd-cmr-perspective.hpp"

gearoenix::render::camera::Camera::Camera(
        const core::Id my_id,
        const std::shared_ptr<system::stream::Stream> &f,
        const std::shared_ptr<engine::Engine> &e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore> &c)
    : core::asset::Asset(my_id, core::asset::Type::CAMERA)
//    , screen_ratio(nullptr == e ? 1.0f : e->get_system_get_window_ratio())
{
}

gearoenix::render::camera::Camera::~Camera()
{
}

void gearoenix::render::camera::Camera::translate(const math::Vec3& vec)
{
//    l += vec;
//    v = v * math::Mat4x4::translator(-vec);
//    vp = p * v;
//    transformed = true;
}

//void gearoenix::render::camera::Camera::move(const math::Vec3& vec)
//{
//    changed = true;
//    l = vec;
//    v = vwl * math::Mat4x4::translator(-vec);
//    vp = p * v;
//}

void gearoenix::render::camera::Camera::local_x_rotate(const core::Real rad)
{
//    math::Mat4x4 rot = math::Mat4x4::rotation(x_axis, -rad);
//    math::Mat4x4 irot = math::Mat4x4::rotation(x_axis, rad);
//    y_axis = irot * y_axis;
//    z_axis = irot * z_axis;
//    vwl = vwl * rot;
//    v = vwl * math::Mat4x4::translator(-l);
//    vp = p * v;
//    transformed = true;
}

void gearoenix::render::camera::Camera::local_y_rotate(const core::Real rad)
{
//    math::Mat4x4 rot = math::Mat4x4::rotation(y_axis, -rad);
//    math::Mat4x4 irot = math::Mat4x4::rotation(y_axis, rad);
//    x_axis = irot * x_axis;
//    z_axis = irot * z_axis;
//    vwl = vwl * rot;
//    v = vwl * math::Mat4x4::translator(-l);
//    vp = p * v;
//    transformed = true;
}

void gearoenix::render::camera::Camera::local_z_rotate(const core::Real rad)
{
//    math::Mat4x4 rot = math::Mat4x4::rotation(z_axis, -rad);
//    math::Mat4x4 irot = math::Mat4x4::rotation(z_axis, rad);
//    x_axis = irot * x_axis;
//    y_axis = irot * y_axis;
//    vwl = vwl * rot;
//    v = vwl * math::Mat4x4::translator(-l);
//    vp = p * v;
//    transformed = true;
}

void gearoenix::render::camera::Camera::local_rotate(const core::Real rad, const math::Vec3& vec)
{
//    math::Mat4x4 rot = math::Mat4x4::rotation(vec, -rad);
//    math::Mat4x4 irot = math::Mat4x4::rotation(vec, rad);
//    x_axis = irot * x_axis;
//    y_axis = irot * y_axis;
//    z_axis = irot * z_axis;
//    vwl = vwl * rot;
//    v = vwl * math::Mat4x4::translator(-l);
//    vp = p * v;
//    transformed = true;
}

void gearoenix::render::camera::Camera::global_rotate(const core::Real rad, const math::Vec3& axis, const math::Vec3& location)
{
//    math::Mat4x4 rot = math::Mat4x4::rotation(axis, -rad);
//    math::Mat4x4 irot = math::Mat4x4::rotation(axis, rad);
//    x_axis = irot * x_axis;
//    y_axis = irot * y_axis;
//    z_axis = irot * z_axis;
//    l = (irot * (l - location)) + location;
//    vwl = vwl * rot;
//    v = vwl * math::Mat4x4::translator(-l);
//    vp = p * v;
//    transformed = true;
}

void gearoenix::render::camera::Camera::global_rotate(const core::Real rad, const math::Vec3& axis)
{
//    math::Mat4x4 rot = math::Mat4x4::rotation(axis, -rad);
//    math::Mat4x4 irot = math::Mat4x4::rotation(axis, rad);
//    x_axis = irot * x_axis;
//    y_axis = irot * y_axis;
//    z_axis = irot * z_axis;
//    vwl = vwl * rot;
//    v = vwl * math::Mat4x4::translator(-l);
//    vp = p * v;
//    transformed = true;
}

void gearoenix::render::camera::Camera::global_rotate(const math::Mat4x4& rm)
{
//    GXUNEXPECTED; // it has performance penalty
//    math::Mat4x4 irot = rm.inversed();
//    x_axis = irot * x_axis;
//    y_axis = irot * y_axis;
//    z_axis = irot * z_axis;
//    l = irot * l;
//    vwl = vwl * rm;
//    v = vwl * math::Mat4x4::translator(-l);
//    vp = p * v;
//    transformed = true;
}

const gearoenix::math::Mat4x4& gearoenix::render::camera::Camera::get_view_projection_matrix() const
{
//    return vp;
}

const gearoenix::math::Mat4x4& gearoenix::render::camera::Camera::get_zero_located_view() const
{
//    return vwl;
}

void gearoenix::render::camera::Camera::get_location(math::Vec3& location) const
{
//    location = l;
}

void gearoenix::render::camera::Camera::set_location(const math::Vec3&)
{
    GXUNIMPLEMENTED;
}

void gearoenix::render::camera::Camera::look_at(const math::Vec3& target, const math::Vec3& up)
{
//    z_axis = (target - l).normalized();
//    x_axis = up.cross(z_axis);
//    y_axis - x_axis.cross(y_axis);
//    vwl = math::Mat4x4::look_at(l, target, up);
//    v = vwl * math::Mat4x4::translator(-l);
//    vp = p * v;
//    transformed = true;
}

void gearoenix::render::camera::Camera::look_at(const math::Vec3& origin, const math::Vec3& target, const math::Vec3& up)
{
//    l = origin;
//    z_axis = (target - l).normalized();
//    x_axis = up.cross(z_axis);
//    y_axis - x_axis.cross(y_axis);
//    vwl = math::Mat4x4::look_at(l, target, up);
//    v = vwl * math::Mat4x4::translator(-l);
//    vp = p * v;
//    transformed = true;
}

void gearoenix::render::camera::Camera::on_event(const core::event::Event& e)
{
//    if (e.get_type() == core::event::Event::From::WINDOW_RESIZE) {
//        const core::event::WindowResize& event = e.to_window_resize();
//        screen_ratio = event.get_current_width() / event.get_current_height();
//    }
}

void gearoenix::render::camera::Camera::global_scale(const core::Real)
{
    GXUNEXPECTED;
}

void gearoenix::render::camera::Camera::local_scale(const core::Real)
{
    GXUNEXPECTED;
}
