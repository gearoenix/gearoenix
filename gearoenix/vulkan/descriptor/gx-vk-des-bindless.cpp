#include "gx-vk-des-bindless.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../gx-vk-check.hpp"

namespace {
constexpr VkDescriptorBindingFlags gx_sampled_image_binding_flags =
    VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT |
    VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT |
    VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT;
}

gearoenix::vulkan::descriptor::Bindless::Bindless(const device::Logical& logical_device)
    : logical_device(logical_device)
    , sampled_image_infos(max_sampled_images)
{
    VkDescriptorSetLayoutBinding binding;
    GX_SET_ZERO(binding);
    binding.binding = 0;
    binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    binding.descriptorCount = max_sampled_images;
    binding.stageFlags = VK_SHADER_STAGE_ALL;

    VkDescriptorSetLayoutBindingFlagsCreateInfo binding_flags_info;
    GX_SET_ZERO(binding_flags_info);
    binding_flags_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
    binding_flags_info.bindingCount = 1;
    binding_flags_info.pBindingFlags = &gx_sampled_image_binding_flags;

    VkDescriptorSetLayoutCreateInfo layout_info;
    GX_SET_ZERO(layout_info);
    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.bindingCount = 1;
    layout_info.pBindings = &binding;
    layout_info.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
    layout_info.pNext = &binding_flags_info;

    GX_VK_CHK(vkCreateDescriptorSetLayout(logical_device.get_vulkan_data(), &layout_info, nullptr, &set_layout));

    VkDescriptorPoolSize pool_size;
    GX_SET_ZERO(pool_size);
    pool_size.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    pool_size.descriptorCount = max_sampled_images;

    VkDescriptorPoolCreateInfo pool_info;
    GX_SET_ZERO(pool_info);
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = 1;
    pool_info.pPoolSizes = &pool_size;
    pool_info.maxSets = 1;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT | VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

    GX_VK_CHK(vkCreateDescriptorPool(logical_device.get_vulkan_data(), &pool_info, nullptr, &descriptor_pool));

    VkDescriptorSetAllocateInfo allocate_info;
    GX_SET_ZERO(allocate_info);
    allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocate_info.descriptorPool = descriptor_pool;
    allocate_info.descriptorSetCount = 1;
    allocate_info.pSetLayouts = &set_layout;

    GX_VK_CHK(vkAllocateDescriptorSets(logical_device.get_vulkan_data(), &allocate_info, &descriptor_set));

    free_sampled_indices.reserve(max_sampled_images);
    for (std::uint32_t i = max_sampled_images; i > 0; --i) {
        free_sampled_indices.push_back(i - 1);
    }
}

gearoenix::vulkan::descriptor::Bindless::~Bindless()
{
    if (descriptor_set != VK_NULL_HANDLE && descriptor_pool != VK_NULL_HANDLE) {
        vkFreeDescriptorSets(logical_device.get_vulkan_data(), descriptor_pool, 1, &descriptor_set);
    }
    if (descriptor_pool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(logical_device.get_vulkan_data(), descriptor_pool, nullptr);
    }
    if (set_layout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(logical_device.get_vulkan_data(), set_layout, nullptr);
    }
}

VkDescriptorSetLayout gearoenix::vulkan::descriptor::Bindless::get_set_layout() const
{
    return set_layout;
}

VkDescriptorSet gearoenix::vulkan::descriptor::Bindless::get_descriptor_set() const
{
    return descriptor_set;
}

void gearoenix::vulkan::descriptor::Bindless::write_descriptor(
    const std::uint32_t index,
    const VkDescriptorImageInfo& info)
{
    VkWriteDescriptorSet write;
    GX_SET_ZERO(write);
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = descriptor_set;
    write.dstBinding = 0;
    write.dstArrayElement = index;
    write.descriptorCount = 1;
    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write.pImageInfo = &info;
    vkUpdateDescriptorSets(logical_device.get_vulkan_data(), 1, &write, 0, nullptr);
}

std::optional<std::uint32_t> gearoenix::vulkan::descriptor::Bindless::allocate_sampled_image(
    const VkImageView view,
    const VkSampler sampler,
    const VkImageLayout layout)
{
    std::lock_guard<std::mutex> _lg(sampled_allocation_lock);
    if (free_sampled_indices.empty()) {
        return std::nullopt;
    }
    const auto index = free_sampled_indices.back();
    free_sampled_indices.pop_back();

    VkDescriptorImageInfo info;
    GX_SET_ZERO(info);
    info.sampler = sampler;
    info.imageView = view;
    info.imageLayout = layout;
    sampled_image_infos[index] = info;
    write_descriptor(index, info);
    return index;
}

void gearoenix::vulkan::descriptor::Bindless::free_sampled_image(const std::uint32_t index)
{
    std::lock_guard<std::mutex> _lg(sampled_allocation_lock);
    if (index >= sampled_image_infos.size()) {
        return;
    }
    auto& tracked_info = sampled_image_infos[index];
    if (tracked_info.sampler == VK_NULL_HANDLE && tracked_info.imageView == VK_NULL_HANDLE) {
        return;
    }
    tracked_info = {};
    free_sampled_indices.push_back(index);
}

#endif
