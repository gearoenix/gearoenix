#include "gx-vk-qu-pool.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../gx-vk-check.hpp"

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

#endif