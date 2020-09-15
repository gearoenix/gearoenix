#include "gx-vk-sync-semaphore.hpp"
#ifdef USE_VULKAN
#include "../../core/gx-cr-static.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../gx-vk-check.hpp"
#include "../gx-vk-instance.hpp"

gearoenix::render::sync::Semaphore::Semaphore(device::Logical* logical_device)
    : logical_device(logical_device)
{
    const device::Physical* p = logical_device->get_physical_device();
    const Linker* l = p->get_instance()->get_linker();
    VkSemaphoreCreateInfo semaphore_create_info;
    setz(semaphore_create_info);
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VKC(l->vkCreateSemaphore(logical_device->get_vulkan_data(),
        &semaphore_create_info, nullptr, &vulkan_data));
}

gearoenix::render::sync::Semaphore::~Semaphore()
{
    const device::Physical* p = logical_device->get_physical_device();
    const Linker* l = p->get_instance()->get_linker();
    l->vkDestroySemaphore(logical_device->get_vulkan_data(), vulkan_data,
        nullptr);
}

const gearoenix::render::device::Logical* gearoenix::render::sync::Semaphore::get_logical_device() const
{
    return logical_device;
}

const VkSemaphore& gearoenix::render::sync::Semaphore::get_vulkan_data() const
{
    return vulkan_data;
}
#endif
