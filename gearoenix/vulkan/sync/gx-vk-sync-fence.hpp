#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"
#include <memory>
#include <vector>

namespace gearoenix::vulkan::device {
struct Logical;
}

namespace gearoenix::vulkan::engine {
struct Engine;
}

namespace gearoenix::vulkan::sync {
struct Fence {
    GX_GET_CRRF_PRV(device::Logical, logical_device);
    GX_GET_VAL_PRV(VkFence, vulkan_data, nullptr);

public:
    Fence(const Fence&) = delete;
    Fence(Fence&&) = delete;
    Fence& operator=(const Fence&) = delete;
    Fence& operator=(Fence&&) = delete;
    explicit Fence(const device::Logical& logical_device, bool signaled = false);
    ~Fence();
    void wait();
    void reset();
    [[nodiscard]] static std::vector<std::shared_ptr<Fence>> create_frame_based(
        const engine::Engine& e, bool signaled = false);
};
}
#endif