#include "gx-vk-sync-semaphore.hpp"
#ifdef GX_USE_VULKAN
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../gx-vk-check.hpp"

gearoenix::vulkan::sync::Semaphore::Semaphore(std::shared_ptr<device::Logical> ld) noexcept
    : logical_device(std::move(ld))
{
    VkSemaphoreCreateInfo semaphore_create_info;
    GX_SET_ZERO(semaphore_create_info);
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    GX_VK_CHK(vkCreateSemaphore(logical_device->get_vulkan_data(), &semaphore_create_info, nullptr, &vulkan_data))
}

gearoenix::vulkan::sync::Semaphore::~Semaphore() noexcept
{
    Loader::vkDestroySemaphore(logical_device->get_vulkan_data(), vulkan_data, nullptr);
}

#endif
