#ifndef GEAROENIX_VULKAN_QUERY_POOL_HPP
#define GEAROENIX_VULKAN_QUERY_POOL_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../platform/macro/gx-plt-mcr-lock.hpp"
#include "../gx-vk-loader.hpp"
#include <cstdint>
#include <map>

namespace gearoenix::vulkan::device {
struct Logical;
}

namespace gearoenix::vulkan::command {
struct Buffer;
}

namespace gearoenix::vulkan::query {
struct Pool final {
    GX_GET_CRRF_PRV(device::Logical, logical_device)
    GX_GET_VAL_PRV(VkQueryPool, vulkan_data, nullptr)
private:
    GX_CREATE_GUARD(indices)
    std::uint32_t latest_id = 0;
    std::map<VkQueryType, std::map<std::uint32_t, std::uint32_t>> indices;

    [[nodiscard]] std::uint32_t register_request(VkQueryType, std::uint32_t id) noexcept;

public:
    Pool(const device::Logical& logical_device, VkQueryType, std::uint32_t = 1) noexcept;
    ~Pool() noexcept;
    Pool(Pool&&) = delete;
    Pool(const Pool&) = delete;
    Pool& operator=(Pool&&) = delete;
    Pool& operator=(const Pool&) = delete;
    void issue_acceleration_structure_compacted_size(command::Buffer&, VkAccelerationStructureKHR, std::uint32_t id = 0xFFFFFFFF) noexcept;
    [[nodiscard]] VkDeviceSize get_acceleration_structure_compacted_size(std::uint32_t id = 0xFFFFFFFF) noexcept;
};
}

#endif
#endif