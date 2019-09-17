#include "phs-anm-animation.hpp"
#include "../../core/cr-build-configuration.hpp"
#include "../../system/sys-log.hpp"

gearoenix::physics::animation::Animation::Animation(
    const std::function<void(core::Real, core::Real)>& a, 
    const core::Real d, 
    const std::function<void(core::Real)>& del) noexcept
    : duration(d)
    , action(a)
    , on_delete(del)
{
}

gearoenix::physics::animation::Animation::~Animation()
{
    if (!activity) return;
    std::chrono::duration<core::Real> dt(std::chrono::high_resolution_clock::now() - start);
    on_delete(dt.count());
}

bool gearoenix::physics::animation::Animation::apply(
    const std::chrono::high_resolution_clock::time_point& now, 
    const core::Real delta_seconds) noexcept
{
    if (!activity) return true;
    const core::Real d = std::chrono::duration<core::Real>(now - start).count();
    if (d > duration) return true;
    action(d, delta_seconds);
    return false;
}

