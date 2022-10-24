#include "gx-vk-des-pool.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-counter.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../gx-vk-check.hpp"

gearoenix::vulkan::descriptor::Pool::Pool(
    const device::Logical& logical_device,
    const VkDescriptorPoolSize* pool_sizes,
    const std::uint32_t count) noexcept
    : logical_device(logical_device)
{
    VkDescriptorPoolCreateInfo info;
    GX_SET_ZERO(info);
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    info.poolSizeCount = count;
    info.pPoolSizes = pool_sizes;
    info.maxSets = 1000 * count;
    info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    GX_VK_CHK(vkCreateDescriptorPool(logical_device.get_vulkan_data(), &info, nullptr, &vulkan_data));
}

gearoenix::vulkan::descriptor::Pool* gearoenix::vulkan::descriptor::Pool::create_imgui(
    const device::Logical& logical_device) noexcept
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
    return new Pool(logical_device, pool_sizes, GX_COUNT_OF(pool_sizes));
}

gearoenix::vulkan::descriptor::Pool::~Pool() noexcept
{
    vkDestroyDescriptorPool(logical_device.get_vulkan_data(), vulkan_data, nullptr);
}

#endif
