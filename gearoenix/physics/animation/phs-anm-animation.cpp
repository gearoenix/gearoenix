#include "phs-anm-animation.hpp"
#include "../../core/cr-build-configuration.hpp"
#include "../../system/sys-log.hpp"

gearoenix::core::Id gearoenix::physics::animation::Animation::last_id = 0;

gearoenix::physics::animation::Animation::Animation(Type t, const std::function<void(core::Real)>& a, const std::chrono::milliseconds& d)
    : animation_type(t)
    , action(a)
    , start(std::chrono::steady_clock::now())
    , duration(std::chrono::duration_cast<std::chrono::steady_clock::time_point::duration>(d))
    , end(start + duration)
{
}

gearoenix::physics::animation::Animation::~Animation() {}

void gearoenix::physics::animation::Animation::set_start(const std::chrono::steady_clock::time_point& t)
{
#ifdef DEBUG_MODE
    if (start > end)
        UNEXPECTED;
#endif
    start = t;
    end = start + duration;
}

void gearoenix::physics::animation::Animation::set_end(const std::chrono::steady_clock::time_point& t)
{
#ifdef DEBUG_MODE
    if (start > end)
        UNEXPECTED;
#endif
    end = t;
    duration = end - start;
}

void gearoenix::physics::animation::Animation::set_duration(const std::chrono::steady_clock::time_point::duration& d)
{
    duration = d;
    end = start + duration;
}

bool gearoenix::physics::animation::Animation::apply(const std::chrono::steady_clock::time_point& now)
{
    action(std::chrono::duration_cast<std::chrono::duration<core::Real>>(now - start).count());
    return false;
}

gearoenix::physics::animation::Animation::Type gearoenix::physics::animation::Animation::get_type() const
{
    return animation_type;
}
