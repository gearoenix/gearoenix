#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"
#include <memory>

namespace gearoenix::vulkan::device {
struct Logical;
}

namespace gearoenix::vulkan::descriptor {
struct Manager;
struct PoolManager;
struct Set;
struct Pool final {
    friend struct PoolManager;
    friend struct Manager;

    GX_GET_CRRF_PRV(device::Logical, logical_device);
    GX_GET_VAL_PRV(VkDescriptorPool, vulkan_data, nullptr);

    Pool(
        const device::Logical& logical_device,
        const VkDescriptorPoolSize* pool_sizes,
        std::uint32_t count);

    [[nodiscard]] static Pool* create_imgui(const device::Logical& logical_device);

public:
    Pool(Pool&&) = delete;
    Pool(const Pool&) = delete;
    Pool& operator=(Pool&&) = delete;
    Pool& operator=(const Pool&) = delete;
    ~Pool();
};
}
#endif