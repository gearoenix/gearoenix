#include "phs-anm-animation.hpp"
#include "../../core/cr-build-configuration.hpp"
#include "../../system/sys-log.hpp"

volatile std::atomic<gearoenix::core::Id> gearoenix::physics::animation::Animation::last_id(0);

gearoenix::physics::animation::Animation::Animation(Type t, const std::function<void(core::Real, core::Real)>& a, const std::chrono::milliseconds& d, std::function<void()> on_delete)
    : my_id(last_id.fetch_add(1))
    , animation_type(t)
    , action(a)
    , on_delete(on_delete)
    , start(std::chrono::steady_clock::now())
    , duration(std::chrono::duration_cast<std::chrono::steady_clock::time_point::duration>(d))
    , end(start + duration)
{
}

gearoenix::physics::animation::Animation::~Animation()
{
    on_delete();
}

void gearoenix::physics::animation::Animation::set_start(const std::chrono::steady_clock::time_point& t)
{
#ifdef DEBUG_MODE
    if (start > end)
        UNEXPECTED;
#endif
    if (start < t)
        ended = false;
    start = t;
    end = start + duration;
}

void gearoenix::physics::animation::Animation::set_end(const std::chrono::steady_clock::time_point& t)
{
#ifdef DEBUG_MODE
    if (start > end)
        UNEXPECTED;
#endif
    if (end < t)
        ended = false;
    end = t;
    duration = end - start;
}

void gearoenix::physics::animation::Animation::set_duration(const std::chrono::steady_clock::time_point::duration& d)
{
    if (duration < d)
        ended = false;
    duration = d;
    end = start + duration;
}

bool gearoenix::physics::animation::Animation::apply(
    const std::chrono::steady_clock::time_point& now,
    const core::Real delta_millisecond)
{
    action(std::chrono::duration_cast<std::chrono::duration<core::Real>>(now - start).count(), delta_millisecond);
    return false;
}

gearoenix::core::Id gearoenix::physics::animation::Animation::get_id() const
{
    return my_id;
}

gearoenix::physics::animation::Animation::Type gearoenix::physics::animation::Animation::get_type() const
{
    return animation_type;
}

bool gearoenix::physics::animation::Animation::is_ended() const
{
    return ended;
}

void gearoenix::physics::animation::Animation::set_on_delete(std::function<void()> f)
{
    on_delete = f;
}
