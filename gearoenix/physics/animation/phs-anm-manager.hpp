#ifndef GEAROENIX_PHYSICS_ANIMATION_MANAGER_HPP
#define GEAROENIX_PHYSICS_ANIMATION_MANAGER_HPP

#include "../../core/cr-static.hpp"
#include "../../core/cr-types.hpp"
#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <set>
#include <vector>

namespace gearoenix::core::sync {
class KernelWorkers;
}

namespace gearoenix::physics::animation {
struct Animation;
struct Manager {
private:
    struct Kernel {
        std::vector<std::shared_ptr<Animation>> deleted_animations;
    };
    std::mutex animations_guard;
    std::set<std::shared_ptr<Animation>> animations;
    std::vector<Kernel> kernels;
    std::chrono::high_resolution_clock::time_point time = std::chrono::high_resolution_clock::now();
    core::Real duration = 0.0f;

public:
    Manager(core::sync::KernelWorkers* kernels) noexcept;
    ~Manager() noexcept;
    Manager(const Manager& o) noexcept = delete;
    void operator=(const Manager& o) noexcept = delete;
    void add(const std::shared_ptr<Animation>& a) noexcept;
};
}
#endif