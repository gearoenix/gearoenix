#include "phs-anm-once.hpp"

gearoenix::physics::animation::Once::Once(
    const std::function<void(core::Real, core::Real)>& action,
    const std::chrono::milliseconds& duration,
    std::function<void()> on_delete)
    : Animation(Animation::ONCE, action, duration, on_delete)
{
}

gearoenix::physics::animation::Once::~Once()
{
}

bool gearoenix::physics::animation::Once::apply(const std::chrono::steady_clock::time_point& now, const core::Real delta_millisecond)
{
    if (now > end) {
        ended = true;
        return true;
    }
    return Animation::apply(now, delta_millisecond);
}
