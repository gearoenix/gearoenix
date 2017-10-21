#include "rnd-cmr-perspective.hpp"
#include "../../system/sys-file.hpp"
#include <cmath>

gearoenix::render::camera::Perspective::Perspective(system::File* f, system::Application* app)
    : Camera(f, app)
{
    v[14] = -v[14];
    f->read(h_angle);
    c_width = std::tan(h_angle) * start;
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

bool gearoenix::render::camera::Perspective::in_sight(const math::Vec3& location, const core::Real radius)
{
    math::Vec3 el = location - l;
    core::Real ezl = z.dot(el);
    math::Vec3 ez = z * ezl;
    math::Vec3 lez = location - ez;
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
