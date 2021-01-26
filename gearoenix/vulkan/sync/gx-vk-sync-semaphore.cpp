#include "gx-vk-sync-semaphore.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../gx-vk-check.hpp"

gearoenix::vulkan::sync::Semaphore::Semaphore(Semaphore&& o) noexcept
    : logical_device(o.logical_device)
    , vulkan_data(o.vulkan_data)
{
    o.vulkan_data = nullptr;
}

gearoenix::vulkan::sync::Semaphore::Semaphore(const device::Logical& ld) noexcept
    : logical_device(ld)
{
    VkSemaphoreCreateInfo info;
    GX_SET_ZERO(info)
    info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    GX_VK_CHK_L(vkCreateSemaphore(logical_device.get_vulkan_data(), &info, nullptr, &vulkan_data))
}

gearoenix::vulkan::sync::Semaphore::~Semaphore() noexcept
{
    if (nullptr == vulkan_data) {
        Loader::vkDestroySemaphore(logical_device.get_vulkan_data(), vulkan_data, nullptr);
        vulkan_data = nullptr;
    }
}

const VkSemaphore* gearoenix::vulkan::sync::Semaphore::get_vulkan_data_ptr() const noexcept
{
    return &vulkan_data;
}

#endif
