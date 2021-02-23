#ifndef GEAROENIX_VULKAN_QUERY_POOL_HPP
#define GEAROENIX_VULKAN_QUERY_POOL_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"
#include <cstdint>

namespace gearoenix::vulkan::device {
struct Logical;
}

namespace gearoenix::vulkan::query {
struct Pool final {
    GX_GET_CRRF_PRV(device::Logical, logical_device)
    GX_GET_VAL_PRV(VkQueryPool, vulkan_data, nullptr)

public:
    Pool(const device::Logical& logical_device, VkQueryType, std::uint32_t = 1) noexcept;
    ~Pool() noexcept;
    Pool(Pool&&) = delete;
    Pool(const Pool&) = delete;
    Pool& operator=(Pool&&) = delete;
    Pool& operator=(const Pool&) = delete;
};
}

#endif
#endif