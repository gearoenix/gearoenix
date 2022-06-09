#include "gx-vk-sync-fence.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-check.hpp"

gearoenix::vulkan::sync::Fence::Fence(const device::Logical& ld, const bool signaled) noexcept
    : logical_device(ld)
{
    VkFenceCreateInfo fence_create_info;
    GX_SET_ZERO(fence_create_info)
    fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    if (signaled) {
        fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    }
    GX_VK_CHK(vkCreateFence(logical_device.get_vulkan_data(), &fence_create_info, nullptr, &vulkan_data))
}

gearoenix::vulkan::sync::Fence::~Fence() noexcept
{
    vkDestroyFence(logical_device.get_vulkan_data(), vulkan_data, nullptr);
    vulkan_data = nullptr;
}

void gearoenix::vulkan::sync::Fence::wait() noexcept
{
    GX_VK_CHK(vkWaitForFences(logical_device.get_vulkan_data(), 1, &vulkan_data, VK_TRUE, UINT64_MAX))
}

void gearoenix::vulkan::sync::Fence::reset() noexcept {
    GX_VK_CHK(vkResetFences(logical_device.get_vulkan_data(), 1, &vulkan_data))
}

std::vector<std::shared_ptr<gearoenix::vulkan::sync::Fence>> gearoenix::vulkan::sync::Fence::create_frame_based(
    const engine::Engine& e, const bool signaled) noexcept
{
    std::vector<std::shared_ptr<Fence>> result(e.get_swapchain().get_image_views().size());
    for (auto& s : result)
        s = std::make_shared<Fence>(e.get_logical_device(), signaled);
    return result;
}

#endif
