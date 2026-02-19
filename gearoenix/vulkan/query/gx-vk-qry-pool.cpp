#include "gx-vk-qry-pool.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../command/gx-vk-cmd-buffer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../gx-vk-check.hpp"

std::uint32_t gearoenix::vulkan::query::Pool::register_request(const VkQueryType qt, const std::uint64_t ii)
{
    std::lock_guard<std::mutex> _lg(indices_lock);
    auto q_search = indices.find(qt);
    if (indices.end() == q_search) {
        indices[qt][ii] = latest_id;
    } else {
        auto& q_map = q_search->second;
        auto i_search = q_map.find(ii);
        if (q_map.end() != i_search)
            GX_LOG_F("Query with type '" << qt << "' and request-id: '" << ii << "' already exists!");
        q_map.emplace(ii, latest_id);
    }
    return latest_id++;
}

gearoenix::vulkan::query::Pool::Pool(const VkQueryType t, const std::uint32_t c)
{
    VkQueryPoolCreateInfo info;
    GX_SET_ZERO(info);
    info.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
    info.queryCount = c;
    info.queryType = t;
    GX_VK_CHK(vkCreateQueryPool(device::Logical::get().get_vulkan_data(), &info, nullptr, &vulkan_data));
}

gearoenix::vulkan::query::Pool::~Pool()
{
    if (nullptr != vulkan_data) {
        vkDestroyQueryPool(device::Logical::get().get_vulkan_data(), vulkan_data, nullptr);
        vulkan_data = nullptr;
    }
}

void gearoenix::vulkan::query::Pool::issue_acceleration_structure_compacted_size(command::Buffer& cmd, const VkAccelerationStructureKHR accel, const std::uint64_t id)
{
    constexpr auto query_type = VK_QUERY_TYPE_ACCELERATION_STRUCTURE_COMPACTED_SIZE_KHR;
    const auto qi = register_request(query_type, id);
    vkCmdResetQueryPool(cmd.get_vulkan_data(), vulkan_data, qi, 1);
    vkCmdWriteAccelerationStructuresPropertiesKHR(cmd.get_vulkan_data(), 1, &accel, query_type, vulkan_data, qi);
}

VkDeviceSize gearoenix::vulkan::query::Pool::get_acceleration_structure_compacted_size(const std::uint64_t id)
{
    VkDeviceSize result = 0;
    GX_VK_CHK(vkGetQueryPoolResults(device::Logical::get().get_vulkan_data(), vulkan_data, indices[VK_QUERY_TYPE_ACCELERATION_STRUCTURE_COMPACTED_SIZE_KHR][id], 1, sizeof(VkDeviceSize), &result, sizeof(VkDeviceSize), VK_QUERY_RESULT_WAIT_BIT));
    return result;
}

#endif