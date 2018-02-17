#include "rnd-cmr-camera.hpp"
#include "../../core/event/cr-ev-event.hpp"
#include "../../core/event/cr-ev-window-resize.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-log.hpp"
#include "rnd-cmr-orthographic.hpp"
#include "rnd-cmr-perspective.hpp"

gearoenix::render::camera::Camera::Camera(system::stream::Stream* f, system::Application* sysapp)
    : screen_ratio(nullptr == sysapp ? 1.0f : sysapp->get_window_ratio())
    , vwl(math::Mat4x4::look_at(
          math::Vec3(0.0f, 0.0f, 0.0f),
          math::Vec3(0.0f, 0.0f, -1.0f),
          math::Vec3(0.0f, 1.0f, 0.0f)))
{
    l.read(f);
    float d;
    f->read(d);
    math::Mat4x4 r = math::Mat4x4::rotation(math::Vec3::X, d);
    math::Mat4x4 rr = math::Mat4x4::rotation(math::Vec3::X, -d);
    f->read(d);
    r = math::Mat4x4::rotation(math::Vec3::Y, d) * r;
    rr *= math::Mat4x4::rotation(math::Vec3::Y, -d);
    f->read(d);
    r = math::Mat4x4::rotation(math::Vec3::Z, d) * r;
    rr *= math::Mat4x4::rotation(math::Vec3::Z, -d);
    x = r * math::Vec3::X;
    y = r * math::Vec3::Y;
    z = r * math::Vec3::Z;
    vwl *= rr;
    v = vwl * math::Mat4x4::translator(-l);
    f->read(start);
    f->read(end);
}

gearoenix::render::camera::Camera::~Camera()
{
}

gearoenix::render::camera::Camera* gearoenix::render::camera::Camera::read(system::stream::Stream* f, system::Application* sysapp)
{
    core::Id camt;
    f->read(camt);
    switch (camt) {
    case 1:
        return new Perspective(f, sysapp);
        break;
    case 2:
        return new Orthographic(f, sysapp);
    default:
        GXLOGF("Unexpected");
        break;
    }
}

void gearoenix::render::camera::Camera::translate(const math::Vec3& vec)
{
    changed = true;
    l += vec;
    v = v * math::Mat4x4::translator(-vec);
    vp = p * v;
}

void gearoenix::render::camera::Camera::move(const math::Vec3& vec)
{
    changed = true;
    l = vec;
    v = vwl * math::Mat4x4::translator(-vec);
    vp = p * v;
}

void gearoenix::render::camera::Camera::rotate_local_x(const core::Real rad)
{
    changed = true;
    math::Mat4x4 rot = math::Mat4x4::rotation(x, -rad);
    math::Mat4x4 irot = math::Mat4x4::rotation(x, rad);
    y = irot * y;
    z = irot * z;
    vwl = vwl * rot;
    v = vwl * math::Mat4x4::translator(-l);
    vp = p * v;
}

void gearoenix::render::camera::Camera::rotate_local_y(const core::Real rad)
{
    changed = true;
    math::Mat4x4 rot = math::Mat4x4::rotation(y, -rad);
    math::Mat4x4 irot = math::Mat4x4::rotation(y, rad);
    x = irot * x;
    z = irot * z;
    vwl = vwl * rot;
    v = vwl * math::Mat4x4::translator(-l);
    vp = p * v;
}

void gearoenix::render::camera::Camera::rotate_local_z(const core::Real rad)
{
    changed = true;
    math::Mat4x4 rot = math::Mat4x4::rotation(z, -rad);
    math::Mat4x4 irot = math::Mat4x4::rotation(z, rad);
    x = irot * x;
    y = irot * y;
    vwl = vwl * rot;
    v = vwl * math::Mat4x4::translator(-l);
    vp = p * v;
}

void gearoenix::render::camera::Camera::rotate_local(const core::Real rad, const math::Vec3& vec)
{
    changed = true;
    math::Mat4x4 rot = math::Mat4x4::rotation(vec, -rad);
    math::Mat4x4 irot = math::Mat4x4::rotation(vec, rad);
    x = irot * x;
    y = irot * y;
    z = irot * z;
    vwl = vwl * rot;
    v = vwl * math::Mat4x4::translator(-l);
    vp = p * v;
}

void gearoenix::render::camera::Camera::rotate_global_x(const core::Real rad)
{
    changed = true;
    math::Mat4x4 rot = math::Mat4x4::rotation(math::Vec3::X, -rad);
    math::Mat4x4 irot = math::Mat4x4::rotation(math::Vec3::X, rad);
    x = irot * x;
    y = irot * y;
    z = irot * z;
    vwl = vwl * rot;
    v = vwl * math::Mat4x4::translator(-l);
    vp = p * v;
}

void gearoenix::render::camera::Camera::rotate_global_y(const core::Real rad)
{
    changed = true;
    math::Mat4x4 rot = math::Mat4x4::rotation(math::Vec3::Y, -rad);
    math::Mat4x4 irot = math::Mat4x4::rotation(math::Vec3::Y, rad);
    x = irot * x;
    y = irot * y;
    z = irot * z;
    vwl = vwl * rot;
    v = vwl * math::Mat4x4::translator(-l);
    vp = p * v;
}

void gearoenix::render::camera::Camera::rotate_global_z(const core::Real rad)
{
    changed = true;
    math::Mat4x4 rot = math::Mat4x4::rotation(math::Vec3::Z, -rad);
    math::Mat4x4 irot = math::Mat4x4::rotation(math::Vec3::Z, rad);
    x = irot * x;
    y = irot * y;
    z = irot * z;
    vwl = vwl * rot;
    v = vwl * math::Mat4x4::translator(-l);
    vp = p * v;
}

void gearoenix::render::camera::Camera::rotate_global(const core::Real rad, const math::Vec3& vec)
{
    changed = true;
    math::Mat4x4 rot = math::Mat4x4::rotation(vec, -rad);
    math::Mat4x4 rot2 = math::Mat4x4::rotation(vec, rad);
    x = rot2 * x;
    y = rot2 * y;
    z = rot2 * z;
    l = rot2 * l;
    vwl = vwl * rot;
    v = v * rot;
    vp = p * v;
}

const gearoenix::math::Mat4x4& gearoenix::render::camera::Camera::get_view_projection() const
{
    return vp;
}

const gearoenix::math::Mat4x4& gearoenix::render::camera::Camera::get_zero_located_view() const
{
    return vwl;
}

const gearoenix::math::Vec3& gearoenix::render::camera::Camera::get_location() const
{
    return l;
}

const gearoenix::math::Vec3& gearoenix::render::camera::Camera::get_x() const
{
    return x;
}

const gearoenix::math::Vec3& gearoenix::render::camera::Camera::get_y() const
{
    return y;
}

const gearoenix::math::Vec3& gearoenix::render::camera::Camera::get_z() const
{
    return z;
}

void gearoenix::render::camera::Camera::copy_location(math::Vec3& v) const
{
    v[0] = l[0];
    v[1] = l[1];
    v[2] = l[2];
}

void gearoenix::render::camera::Camera::rotate_look_at(const core::Real rad, const math::Vec3& vec, const math::Vec3& point)
{
    changed = true;
    math::Mat4x4 rot = math::Mat4x4::rotation(vec, -rad);
    math::Mat4x4 rot2 = math::Mat4x4::rotation(vec, rad);
    l = (rot2 * (l - point)) + point;
    //	z = normalize(l - p);
    //	float dZgY = dot(z, math::Vec3::Z);
    //	if(dZgY > 0.99)
    //	{
    //		x = math::Vec3::X;
    //		y = math::Vec3::Y;
    //	}
    //	else
    //	{
    //		y = normalize((z * dZgY) + math::Vec3::Z);
    //		x = cross(y, z);
    //	}
    x = rot2 * x;
    y = rot2 * y;
    z = rot2 * z;
    vwl = vwl * rot;
    v = vwl * math::Mat4x4::translator(-l);
    vp = p * v;
}

void gearoenix::render::camera::Camera::move_forward(const core::Real spd)
{
    changed = true;
    math::Vec3 vec = z * spd;
    l += vec;
    v = v * math::Mat4x4::translator(-vec);
    vp = p * v;
}

void gearoenix::render::camera::Camera::move_sideward(const core::Real spd)
{
    changed = true;
    math::Vec3 vec = x * spd;
    l += vec;
    v = vwl * math::Mat4x4::translator(-l);
    vp = p * v;
}

bool gearoenix::render::camera::Camera::get_changed() const
{
    return changed;
}

void gearoenix::render::camera::Camera::clean()
{
    changed = false;
}

void gearoenix::render::camera::Camera::on_event(const core::event::Event& e)
{
    if (e.get_type() == core::event::Event::From::WINDOW_RESIZE) {
        const core::event::WindowResize& event = e.to_window_resize();
        screen_ratio = event.get_current_width() / event.get_current_height();
    }
}
