#include "rnd-cmr-perspective.hpp"
#include "../../system/sys-file.hpp"
#include <cmath>

gearoenix::render::camera::Perspective::Perspective(system::File* f, system::Application* app)
    : Camera(f, app)
{
    f->read(h_angle);
    core::Real w = std::tan(h_angle) * start;
    core::Real h = w / screen_ratio;
    v_angle = std::atan(h / start);
    p = math::Mat4x4::perspective(w, h, start, end);
    vp = p * v;
}

bool gearoenix::render::camera::Perspective::in_sight(const math::Vec3& location, const core::Real radius)
{
    math::Vec3 eye2cnt = location - l;
    math::Vec2 ll(eye2cnt[0], eye2cnt[1]);
    core::Real lll = ll.length();
    ll /= lll;
    math::Vec2 z(this->z[0], this->z[1]);
    core::Real a = std::acos(z.dot(ll));
    core::Real ar = std::asin(radius / lll);
    if (a > ar + h_angle)
        return false;
    ll[0] = eye2cnt[2];
    ll[1] = eye2cnt[1];
    lll = ll.length();
    ll /= lll;
    z[0] = this->z[2];
    z[1] = this->z[1];
    a = std::acos(z.dot(ll));
    ar = std::asin(radius / lll);
    return a < ar + v_angle;
}

void gearoenix::render::camera::Perspective::window_size_changed()
{
}
