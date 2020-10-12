#include "gx-vk-sync-fence.hpp"
#ifdef GX_USE_VULKAN
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../gx-vk-check.hpp"

gearoenix::vulkan::sync::Fence::Fence(std::shared_ptr<device::Logical> ld, const bool signaled) noexcept
    : logical_device(std::move(ld))
{
    VkFenceCreateInfo fence_create_info;
    GX_SET_ZERO(fence_create_info)
    fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    if (signaled) {
        fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    }
    GX_VK_CHK_L(vkCreateFence(logical_device->get_vulkan_data(), &fence_create_info, nullptr, &vulkan_data))
}

gearoenix::vulkan::sync::Fence::~Fence() noexcept
{
    Loader::vkDestroyFence(logical_device->get_vulkan_data(), vulkan_data, nullptr);
}

void gearoenix::vulkan::sync::Fence::wait() noexcept
{
    GX_VK_CHK_L(vkWaitForFences(logical_device->get_vulkan_data(), 1, &vulkan_data, VK_TRUE, UINT64_MAX))
}

void gearoenix::vulkan::sync::Fence::reset() noexcept
{
    GX_VK_CHK_L(vkResetFences(logical_device->get_vulkan_data(), 1, &vulkan_data))
}

#endif
