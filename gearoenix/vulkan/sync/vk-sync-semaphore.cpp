#include "vk-sync-semaphore.hpp"
#include "../../core/static.hpp"
#include "../device/vk-dev-logical.hpp"
#include "../device/vk-dev-physical.hpp"
#include "../vk-instance.hpp"
#include "../vk-check.hpp"

gearoenix::nufrag::render::sync::Semaphore::Semaphore(const std::shared_ptr<device::Logical> &logical_device): logical_device(logical_device) {
    auto &l = logical_device->get_physical_device()->get_instance()->get_linker();
    VkSemaphoreCreateInfo semaphore_create_info;
    setz(semaphore_create_info);
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VKC(l->vkCreateSemaphore(logical_device->get_vulkan_data(), &semaphore_create_info, nullptr, &vulkan_data));
}

gearoenix::nufrag::render::sync::Semaphore::~Semaphore() {
    auto &l = logical_device->get_physical_device()->get_instance()->get_linker();
    l->vkDestroySemaphore(logical_device->get_vulkan_data(), vulkan_data, nullptr);
}

const std::shared_ptr<gearoenix::nufrag::render::device::Logical> &gearoenix::nufrag::render::sync::Semaphore::get_logical_device() const {
    return logical_device;
}

const VkSemaphore &gearoenix::nufrag::render::sync::Semaphore::get_vulkan_data() const {
    return vulkan_data;
}
