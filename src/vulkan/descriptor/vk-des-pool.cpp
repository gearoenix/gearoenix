#include "vk-des-pool.hpp"
#ifdef USE_VULKAN
#include "../../core/cr-static.hpp"
#include "../device/vk-dev-logical.hpp"
#include "../device/vk-dev-physical.hpp"
#include "../vk-check.hpp"
#include "../vk-instance.hpp"

gearoenix::render::descriptor::Pool::Pool(device::Logical* logical_device)
    : logical_device(logical_device)
{
    const device::Physical* p = logical_device->get_physical_device();
    const Linker* l = p->get_instance()->get_linker();
    VkDescriptorPoolSize type_counts[2];
    setz(type_counts[0]);
    type_counts[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    type_counts[0].descriptorCount = 3;
    setz(type_counts[1]);
    type_counts[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    type_counts[1].descriptorCount = 3;
    VkDescriptorPoolCreateInfo descriptor_pool_info;
    setz(descriptor_pool_info);
    descriptor_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptor_pool_info.poolSizeCount = countof(type_counts);
    descriptor_pool_info.pPoolSizes = type_counts;
    descriptor_pool_info.maxSets = 3;
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

gearoenix::render::device::Logical* gearoenix::render::descriptor::Pool::get_logical_device()
{
    return logical_device;
}

const VkDescriptorPool& gearoenix::render::descriptor::Pool::get_vulkan_data()
    const
{
    return vulkan_data;
}
#endif
