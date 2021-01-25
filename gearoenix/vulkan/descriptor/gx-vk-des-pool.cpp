#include "gx-vk-des-pool.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-counter.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../gx-vk-check.hpp"

gearoenix::vulkan::descriptor::Pool::Pool(
    const device::Logical& logical_device,
    VkDescriptorPool vulkan_data) noexcept
    : logical_device(logical_device)
    , vulkan_data(vulkan_data)
{
}

gearoenix::vulkan::descriptor::Pool::Pool(const device::Logical& logical_device) noexcept
    : logical_device(logical_device)
{
    VkDescriptorPoolSize type_counts[2];
    GX_SET_ARRAY_ZERO(type_counts)
    type_counts[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    type_counts[0].descriptorCount = 1000;
    type_counts[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    type_counts[1].descriptorCount = 1000;
    VkDescriptorPoolCreateInfo info;
    GX_SET_ZERO(info)
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    info.poolSizeCount = GX_COUNT_OF(type_counts);
    info.pPoolSizes = type_counts;
    info.maxSets = 1000;
    GX_VK_CHK_L(vkCreateDescriptorPool(logical_device.get_vulkan_data(), &info, nullptr, &vulkan_data))
}

gearoenix::vulkan::descriptor::Pool::~Pool() noexcept
{
    Loader::vkDestroyDescriptorPool(logical_device.get_vulkan_data(), vulkan_data, nullptr);
}

gearoenix::vulkan::descriptor::Pool gearoenix::vulkan::descriptor::Pool::create_imgui(const device::Logical& logical_device) noexcept
{
    const VkDescriptorPoolSize pool_sizes[] = {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };
    VkDescriptorPoolCreateInfo info;
    GX_SET_ZERO(info)
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    info.maxSets = 1000 * GX_COUNT_OF(pool_sizes);
    info.poolSizeCount = GX_COUNT_OF(pool_sizes);
    info.pPoolSizes = pool_sizes;
    VkDescriptorPool vulkan_data;
    GX_VK_CHK_L(vkCreateDescriptorPool(logical_device.get_vulkan_data(), &info, nullptr, &vulkan_data))
    return Pool(logical_device, vulkan_data);
}

#endif
