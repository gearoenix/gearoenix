#include "phs-anm-once.hpp"

gearoenix::physics::animation::Once::Once(const std::function<void(core::Real)>& action, const std::chrono::milliseconds& duration)
    : Animation(Animation::ONCE, action, duration)
{
}

gearoenix::physics::animation::Once::~Once()
{
}

bool gearoenix::physics::animation::Once::apply(const std::chrono::steady_clock::time_point& now)
{
    if (now > end)
        return true;
    return Animation::apply(now);
}
