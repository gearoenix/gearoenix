#include "vk-des-pool.hpp"
#include "../../core/static.hpp"
#include "../device/vk-dev-logical.hpp"
#include "../device/vk-dev-physical.hpp"
#include "../vk-instance.hpp"
#include "../vk-check.hpp"

gearoenix::nufrag::render::descriptor::Pool::Pool(const std::shared_ptr<device::Logical> &logical_device): logical_device(logical_device) {
    auto &l = logical_device->get_physical_device()->get_instance()->get_linker();
    VkDescriptorPoolSize type_counts;
    setz(type_counts);
    type_counts.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    type_counts.descriptorCount = 1;
    VkDescriptorPoolCreateInfo descriptor_pool_info;
    setz(descriptor_pool_info);
    descriptor_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptor_pool_info.poolSizeCount = 1;
    descriptor_pool_info.pPoolSizes = &type_counts;
    descriptor_pool_info.maxSets = 1;
    VKC(l->vkCreateDescriptorPool(logical_device->get_vulkan_data(), &descriptor_pool_info, nullptr, &vulkan_data));
}

gearoenix::nufrag::render::descriptor::Pool::~Pool() {
    auto &l = logical_device->get_physical_device()->get_instance()->get_linker();
    l->vkDestroyDescriptorPool(logical_device->get_vulkan_data(), vulkan_data, nullptr);
}

const std::shared_ptr<gearoenix::nufrag::render::device::Logical> &gearoenix::nufrag::render::descriptor::Pool::get_logical_device() const {
    return logical_device;
}

const VkDescriptorPool &gearoenix::nufrag::render::descriptor::Pool::get_vulkan_data() const {
    return vulkan_data;
}
