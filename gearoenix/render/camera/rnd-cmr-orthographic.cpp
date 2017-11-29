#include "rnd-cmr-orthographic.hpp"
#include "../../system/sys-file.hpp"
#include <cmath>

gearoenix::render::camera::Orthographic::Orthographic(system::File* f, system::Application* sysapp)
    : Camera(f, sysapp)
{
    f->read(aspects_size);
    c_width = aspects_size * screen_ratio;
    c_height = aspects_size;
    p = math::Mat4x4::orthographic(c_width * 2.0f, c_height * 2.0f, start, end);
    vp = p * v;
}

bool gearoenix::render::camera::Orthographic::in_sight(const math::Vec3& location, const core::Real radius) const
{
    math::Vec3 el = location - l;
    math::Vec3 ez = z * (z.dot(el));
    math::Vec3 ezl = location - ez;
    core::Real w = ezl.dot(x);
    core::Real h = ezl.dot(y);
    if (w > c_width + radius)
        return false;
    return h < c_height + radius;
}

void gearoenix::render::camera::Orthographic::window_size_changed()
{
}
