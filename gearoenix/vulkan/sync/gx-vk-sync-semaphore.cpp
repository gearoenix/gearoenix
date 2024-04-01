#include "gx-vk-sync-semaphore.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-check.hpp"

gearoenix::vulkan::sync::Semaphore::Semaphore(Semaphore&& o)
    : logical_device(o.logical_device)
    , vulkan_data(o.vulkan_data)
{
    o.vulkan_data = nullptr;
}

gearoenix::vulkan::sync::Semaphore::Semaphore(const device::Logical& ld)
    : logical_device(ld)
{
    VkSemaphoreCreateInfo info;
    GX_SET_ZERO(info);
    info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    GX_VK_CHK(vkCreateSemaphore(logical_device.get_vulkan_data(), &info, nullptr, &vulkan_data));
}

gearoenix::vulkan::sync::Semaphore::~Semaphore()
{
    if (nullptr != vulkan_data) {
        vkDestroySemaphore(logical_device.get_vulkan_data(), vulkan_data, nullptr);
        vulkan_data = nullptr;
    }
}

const VkSemaphore* gearoenix::vulkan::sync::Semaphore::get_vulkan_data_ptr() const
{
    return &vulkan_data;
}

std::vector<std::shared_ptr<gearoenix::vulkan::sync::Semaphore>> gearoenix::vulkan::sync::Semaphore::create_frame_based(
    const engine::Engine& e)
{
    std::vector<std::shared_ptr<Semaphore>> result(e.get_swapchain().get_image_views().size());
    for (auto& s : result)
        s = std::make_shared<Semaphore>(e.get_logical_device());
    return result;
}

#endif
