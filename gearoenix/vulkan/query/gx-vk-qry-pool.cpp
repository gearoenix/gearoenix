#include "gx-vk-qry-pool.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../command/gx-vk-cmd-buffer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../gx-vk-check.hpp"

std::uint32_t gearoenix::vulkan::query::Pool::register_request(const VkQueryType qt, const std::uint32_t ii) noexcept
{
    GX_GUARD_LOCK(indices)
    auto q_search = indices.find(qt);
    if (indices.end() == q_search) {
        indices[qt][ii] = latest_id;
    } else {
        auto& q_map = q_search->second;
        auto i_search = q_map.find(ii);
        if (q_map.end() != i_search)
            GX_LOG_F("Query with type '" << qt << "' and request-id: '" << ii << "' already exists!")
        q_map.emplace(ii, latest_id);
    }
    const auto id = latest_id;
    ++latest_id;
    return id;
}

gearoenix::vulkan::query::Pool::Pool(
    const device::Logical& logical_device,
    const VkQueryType t,
    const std::uint32_t c) noexcept
    : logical_device(logical_device)
{
    VkQueryPoolCreateInfo info;
    GX_SET_ZERO(info)
    info.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
    info.queryCount = c;
    info.queryType = t;
    GX_VK_CHK(vkCreateQueryPool(logical_device.get_vulkan_data(), &info, nullptr, &vulkan_data))
}

gearoenix::vulkan::query::Pool::~Pool() noexcept
{
    if (nullptr != vulkan_data) {
        vkDestroyQueryPool(logical_device.get_vulkan_data(), vulkan_data, nullptr);
        vulkan_data = nullptr;
    }
}

void gearoenix::vulkan::query::Pool::issue_acceleration_structure_compacted_size(
    command::Buffer& cmd,
    VkAccelerationStructureKHR accel, std::uint32_t issue_id) noexcept
{
    constexpr static const auto query_type = VK_QUERY_TYPE_ACCELERATION_STRUCTURE_COMPACTED_SIZE_KHR;
    vkCmdWriteAccelerationStructuresPropertiesKHR(
        cmd.get_vulkan_data(), 1, &accel, query_type, vulkan_data, register_request(query_type, issue_id));
}

VkDeviceSize gearoenix::vulkan::query::Pool::get_acceleration_structure_compacted_size(const std::uint32_t id) noexcept
{
    VkDeviceSize result = 0;
    GX_VK_CHK(vkGetQueryPoolResults(
        logical_device.get_vulkan_data(), vulkan_data,
        indices[VK_QUERY_TYPE_ACCELERATION_STRUCTURE_COMPACTED_SIZE_KHR][id], 1,
        sizeof(VkDeviceSize), &result, sizeof(VkDeviceSize), VK_QUERY_RESULT_WAIT_BIT))
    return result;
}

#endif