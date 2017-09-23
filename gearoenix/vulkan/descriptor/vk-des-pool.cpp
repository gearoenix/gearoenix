#include "vk-des-pool.hpp"
#include "../../core/cr-static.hpp"
#include "../device/vk-dev-logical.hpp"
#include "../device/vk-dev-physical.hpp"
#include "../vk-check.hpp"
#include "../vk-instance.hpp"

const uint32_t gearoenix::render::descriptor::Pool::TYPE_COUNT = 2;

gearoenix::render::descriptor::Pool::Pool(device::Logical* logical_device)
    : logical_device(logical_device)
{
    const device::Physical* p = logical_device->get_physical_device();
    const Linker* l = p->get_instance()->get_linker();
    VkDescriptorPoolSize type_counts;
    setz(type_counts);
    type_counts.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    type_counts.descriptorCount = TYPE_COUNT;
    VkDescriptorPoolCreateInfo descriptor_pool_info;
    setz(descriptor_pool_info);
    descriptor_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptor_pool_info.poolSizeCount = 1;
    descriptor_pool_info.pPoolSizes = &type_counts;
    descriptor_pool_info.maxSets = TYPE_COUNT;
    VKC(l->vkCreateDescriptorPool(logical_device->get_vulkan_data(),
        &descriptor_pool_info, nullptr, &vulkan_data));
}

gearoenix::render::descriptor::Pool::~Pool()
{
    const device::Physical* p = logical_device->get_physical_device();
    const Linker* l = p->get_instance()->get_linker();
    l->vkDestroyDescriptorPool(logical_device->get_vulkan_data(), vulkan_data,
        nullptr);
}

const gearoenix::render::device::Logical* gearoenix::render::descriptor::Pool::get_logical_device() const
{
    return logical_device;
}

const VkDescriptorPool& gearoenix::render::descriptor::Pool::get_vulkan_data()
    const
{
    return vulkan_data;
}
