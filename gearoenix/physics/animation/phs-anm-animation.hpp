#ifndef GEAROENIX_PHYSICS_ANIMATION_ANIMATION_HPP
#define GEAROENIX_PHYSICS_ANIMATION_ANIMATION_HPP

#include "../../core/cr-static.hpp"
#include "../../core/cr-types.hpp"
#include <chrono>
#include <functional>

namespace gearoenix::physics::animation {
struct Animation {
    GX_GETSET_VAL_PRV(bool, activity, true)
    GX_GETSET_VAL_PRV(double, duration, 0.0)
private:
    // seconds from start, delta time
    std::function<void(double, double)> action;
    std::function<void(double)> on_delete;
    const std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

public:
    Animation(
        std::function<void(double, double)> action,
        double duration,
        std::function<void(double)> on_delete = [](double) noexcept {}) noexcept;
    ~Animation() noexcept;
    Animation(const Animation& other) noexcept = delete;
    void operator=(const Animation& other) noexcept = delete;
    // true means: delete the animation and animation will not apply anymore
    // false means: animation is working and active
    bool apply(const std::chrono::high_resolution_clock::time_point& now, double delta_seconds) noexcept;
};
}
#endif