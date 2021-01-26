#ifndef GEAROENIX_VULKAN_FENCE_HPP
#define GEAROENIX_VULKAN_FENCE_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"

namespace gearoenix::vulkan::device {
struct Logical;
}

namespace gearoenix::vulkan::sync {
struct Fence {
    GX_GET_CRRF_PRV(device::Logical, logical_device)
    GX_GET_VAL_PRV(VkFence, vulkan_data, nullptr)

public:
    Fence(const Fence&) = delete;
    Fence(Fence&&) noexcept;
    explicit Fence(const device::Logical& logical_device, bool signaled = false) noexcept;
    ~Fence() noexcept;
    void wait() noexcept;
    void reset() noexcept;
    Fence& operator=(const Fence&) = delete;
    Fence& operator=(Fence&&) = delete;
};
}
#endif
#endif
