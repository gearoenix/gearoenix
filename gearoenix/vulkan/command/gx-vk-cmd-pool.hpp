#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"

namespace gearoenix::vulkan::device {
struct Logical;
}

namespace gearoenix::vulkan::command {
struct Pool final {
    GX_GET_CRRF_PRV(device::Logical, logical_device);
    GX_GET_VAL_PRV(VkCommandPool, vulkan_data, nullptr);

public:
    Pool(Pool&&) noexcept;
    Pool(const Pool&) = delete;
    explicit Pool(const device::Logical& logical_device);
    ~Pool();
    Pool& operator=(Pool&&) = delete;
    Pool& operator=(const Pool&) = delete;
};
}
#endif