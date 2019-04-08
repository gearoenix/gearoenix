#include "rnd-cmr-camera.hpp"
#include "../../core/event/cr-ev-event.hpp"
#include "../../core/event/cr-ev-window-resize.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-log.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../../math/math-quaternion.hpp"
#include "../buffer/rnd-buf-framed-uniform.hpp"
#include "rnd-cmr-orthographic.hpp"
#include "rnd-cmr-perspective.hpp"

void gearoenix::render::camera::Camera::update_location()
{
    math::Mat4x4 translate = math::Mat4x4::translator(-uniform.position_far.xyz());
    uniform.view = uniform.inversed_rotation * translate;
    update_view_projection();
}

void gearoenix::render::camera::Camera::update_view_projection()
{
    uniform.view_projection = uniform.projection * uniform.view;
    uniform.uniform_view_projection = math::Mat4x4(
                0.5f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.5f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.5f, 0.5f, 0.0f, 1.0f)
            * uniform.view_projection;
}

gearoenix::render::camera::Camera::Camera(
        const core::Id my_id,
        const std::shared_ptr<system::stream::Stream> &f,
        const std::shared_ptr<engine::Engine> &e)
    : core::asset::Asset(my_id, core::asset::Type::CAMERA)
	, uniform_buffers(new buffer::FramedUniform(sizeof(Uniform), e))
{
    uniform.near_aspect_ratio_reserved[1] = e->get_system_application()->get_window_ratio();
    f->read(uniform.position_far[0]);
    f->read(uniform.position_far[1]);
    f->read(uniform.position_far[2]);
    math::Quat q;
    f->read(q.w);
    f->read(q.x);
    f->read(q.y);
    f->read(q.z);
    uniform.near_aspect_ratio_reserved[0]= -f->read<core::Real>();
    uniform.position_far[3]= -f->read<core::Real>();
    const math::Mat4x4 r = q.to_mat();
    uniform.x_reserved.xyz((r * math::Vec4(uniform.x_reserved.xyz(), 0.0f)).xyz());
    uniform.y_reserved.xyz((r * math::Vec4(uniform.y_reserved.xyz(), 0.0f)).xyz());
    uniform.z_reserved.xyz((r * math::Vec4(uniform.z_reserved.xyz(), 0.0f)).xyz());
    uniform.inversed_rotation = math::Quat(q.x, q.y, q.z, -q.w).to_mat();
    GXLOGD("Position and far: " << uniform.position_far);
    GXLOGD("Quaternion: " << q);
    GXLOGD("Near aspect ratio: " << uniform.near_aspect_ratio_reserved);
    update_location();
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
    return uniform.view_projection;
}

const gearoenix::math::Mat4x4& gearoenix::render::camera::Camera::get_zero_located_view() const
{
    return uniform.inversed_rotation;
}

const std::shared_ptr<gearoenix::render::buffer::FramedUniform>& gearoenix::render::camera::Camera::get_uniform_buffers() const
{
	return uniform_buffers;
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

void gearoenix::render::camera::Camera::set_orientation(const math::Quat &q)
{
    const math::Mat4x4 r = q.to_mat();
    uniform.x_reserved.xyz((r * math::Vec4(uniform.x_reserved.xyz(), 0.0f)).xyz());
    uniform.y_reserved.xyz((r * math::Vec4(uniform.y_reserved.xyz(), 0.0f)).xyz());
    uniform.z_reserved.xyz((r * math::Vec4(uniform.z_reserved.xyz(), 0.0f)).xyz());
    uniform.inversed_rotation = math::Quat(q.x, q.y, q.z, -q.w).to_mat();
    update_location();
}
