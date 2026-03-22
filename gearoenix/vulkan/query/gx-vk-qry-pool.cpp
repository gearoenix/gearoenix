#include "gx-vk-qry-pool.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../command/gx-vk-cmd-buffer.hpp"
#include "../device/gx-vk-dev-logical.hpp"

std::uint32_t gearoenix::vulkan::query::Pool::register_request(const vk::QueryType qt, const std::uint64_t ii)
{
    std::lock_guard<std::mutex> _lg(indices_lock);
    auto q_search = indices.find(qt);
    if (indices.end() == q_search) {
        indices[qt][ii] = latest_id;
    } else {
        auto& q_map = q_search->second;
        auto i_search = q_map.find(ii);
        if (q_map.end() != i_search)
            GX_LOG_F("Query with type '" << vk::to_string(qt) << "' and request-id: '" << ii << "' already exists!");
        q_map.emplace(ii, latest_id);
    }
    return latest_id++;
}

gearoenix::vulkan::query::Pool::Pool(const vk::QueryType t, const std::uint32_t c)
    : vulkan_data(device::Logical::get().get_device(), vk::QueryPoolCreateInfo { { }, t, c })
{
}

gearoenix::vulkan::query::Pool::~Pool() = default;

void gearoenix::vulkan::query::Pool::issue_acceleration_structure_compacted_size(command::Buffer& cmd, const vk::AccelerationStructureKHR accel, const std::uint64_t id)
{
    constexpr auto query_type = vk::QueryType::eAccelerationStructureCompactedSizeKHR;
    const auto qi = register_request(query_type, id);
    const auto vk_cmd = cmd.get_vulkan_data();
    vk_cmd.resetQueryPool(vulkan_data, qi, 1);
    vk_cmd.writeAccelerationStructuresPropertiesKHR(accel, query_type, vulkan_data, qi);
}

vk::DeviceSize gearoenix::vulkan::query::Pool::get_acceleration_structure_compacted_size(const std::uint64_t id)
{
    vk::DeviceSize result = 0;
    const auto qi = indices[vk::QueryType::eAccelerationStructureCompactedSizeKHR][id];
    device::Logical::get().get_vulkan_data().getQueryPoolResults(vulkan_data, qi, 1, sizeof(vk::DeviceSize), &result, sizeof(vk::DeviceSize), vk::QueryResultFlagBits::eWait);
    return result;
}

#endif
