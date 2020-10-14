#ifndef GEAROENIX_PHYSICS_ANIMATION_MANAGER_HPP
#define GEAROENIX_PHYSICS_ANIMATION_MANAGER_HPP

#include "../../core/gx-cr-static.hpp"
#include "../../core/gx-cr-types.hpp"
#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <vector>

namespace gearoenix::core::sync {
class KernelWorkers;
}

namespace gearoenix::physics::animation {
struct Animation;
struct Manager {
private:
    struct Kernel {
        std::vector<std::pair<core::Id, core::Id>> deleted_animations;
    };
    GX_CREATE_GUARD(animations)
    std::map<core::Id, std::map<core::Id, std::shared_ptr<Animation>>> animations;
    std::vector<Kernel> kernels;
    std::chrono::high_resolution_clock::time_point time = std::chrono::high_resolution_clock::now();
    double duration = 0.0;

public:
    explicit Manager(core::sync::KernelWorkers* kernels) noexcept;
    ~Manager() noexcept;
    Manager(const Manager& o) noexcept = delete;
    void operator=(const Manager& o) noexcept = delete;
    void add(core::Id animated_root_object_id, const std::shared_ptr<Animation>& a) noexcept;
};
}
#endif