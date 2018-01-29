#include "rnd-anm-animation.hpp"

gearoenix::render::animation::Animation::Animation(Type t, math::Transformable* obj, std::function<void(core::Real)> actioner)
    : t(t)
    , o(obj)
    , a(actioner)
    , s(std::chrono::steady_clock::now())
{
}

gearoenix::render::animation::Animation::~Animation() {}

void gearoenix::render::animation::Animation::start(const std::chrono::steady_clock::time_point& now)
{
    s = now;
}

bool gearoenix::render::animation::Animation::apply(const std::chrono::steady_clock::time_point& now)
{
    std::chrono::duration<core::Real> duration = std::chrono::duration_cast<std::chrono::duration<core::Real>>(now - s);
    a(duration.count());
    return false;
}

gearoenix::render::animation::Animation::Type gearoenix::render::animation::Animation::get_type() const
{
    return t;
}
