#include "phs-anm-animation.hpp"
#include "../../system/sys-log.hpp"
#include <utility>

gearoenix::physics::animation::Animation::Animation(
    std::function<void(double, double)> a,
    const double d,
    std::function<void(double)> del) noexcept
    : duration(d)
    , action(std::move(a))
    , on_delete(std::move(del))
{
}

gearoenix::physics::animation::Animation::~Animation() noexcept
{
    if (!activity)
        return;
    std::chrono::duration<double> dt(std::chrono::high_resolution_clock::now() - start);
    on_delete(dt.count());
}

bool gearoenix::physics::animation::Animation::apply(
    const std::chrono::high_resolution_clock::time_point& now,
    const double delta_seconds) noexcept
{
    if (!activity)
        return true;
    const double d = std::chrono::duration<double>(now - start).count();
    if (d > duration)
        return true;
    action(d, delta_seconds);
    return false;
}
