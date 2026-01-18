#include "gx-vk-des-bindless.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../gx-vk-check.hpp"

namespace {
constexpr VkDescriptorBindingFlags gx_binding_flags =
    VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT |
    VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT |
    VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT;

    constexpr VkDescriptorBindingFlags gx_static_binding_flag_array[] {
        gx_binding_flags, // 1D images
        gx_binding_flags, // 2D images
        gx_binding_flags, // 3D images
        gx_binding_flags, // Cube images
        gx_binding_flags, // Samplers
    };
}

gearoenix::vulkan::descriptor::Bindless::Bindless()
    : image_1d_infos(max_1d_images)
    , image_2d_infos(max_2d_images)
    , image_3d_infos(max_3d_images)
    , image_cube_infos(max_cube_images)
    , sampler_infos(max_samplers)
{
    const auto vk_dev = device::Logical::get().get_vulkan_data();

    VkDescriptorSetLayoutBinding static_bindings[std::size(gx_static_binding_flag_array)];
    GX_SET_ZERO(static_bindings);

    static_bindings[0].binding = 0;
    static_bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    static_bindings[0].descriptorCount = max_1d_images;
    static_bindings[0].stageFlags = VK_SHADER_STAGE_ALL;

    static_bindings[1].binding = 1;
    static_bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    static_bindings[1].descriptorCount = max_2d_images;
    static_bindings[1].stageFlags = VK_SHADER_STAGE_ALL;

    static_bindings[2].binding = 2;
    static_bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    static_bindings[2].descriptorCount = max_3d_images;
    static_bindings[2].stageFlags = VK_SHADER_STAGE_ALL;

    static_bindings[3].binding = 3;
    static_bindings[3].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    static_bindings[3].descriptorCount = max_cube_images;
    static_bindings[3].stageFlags = VK_SHADER_STAGE_ALL;

    static_bindings[4].binding = 4;
    static_bindings[4].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
    static_bindings[4].descriptorCount = max_samplers;
    static_bindings[4].stageFlags = VK_SHADER_STAGE_ALL;

    VkDescriptorSetLayoutBindingFlagsCreateInfo binding_flags_info;
    GX_SET_ZERO(binding_flags_info);
    binding_flags_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
    binding_flags_info.bindingCount = std::size(gx_static_binding_flag_array);
    binding_flags_info.pBindingFlags = gx_static_binding_flag_array;

    VkDescriptorSetLayoutCreateInfo layout_info;
    GX_SET_ZERO(layout_info);
    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.bindingCount = std::size(static_bindings);
    layout_info.pBindings = static_bindings;
    layout_info.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
    layout_info.pNext = &binding_flags_info;

    GX_VK_CHK(vkCreateDescriptorSetLayout(vk_dev, &layout_info, nullptr, &set_layout));

    VkDescriptorPoolSize pool_sizes[std::size(gx_static_binding_flag_array)];
    GX_SET_ZERO(pool_sizes);
    pool_sizes[0].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    pool_sizes[0].descriptorCount = max_1d_images;

    pool_sizes[1].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    pool_sizes[1].descriptorCount = max_2d_images;

    pool_sizes[2].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    pool_sizes[2].descriptorCount = max_3d_images;

    pool_sizes[3].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    pool_sizes[3].descriptorCount = max_cube_images;
    
    pool_sizes[4].type = VK_DESCRIPTOR_TYPE_SAMPLER;
    pool_sizes[4].descriptorCount = max_samplers;

    VkDescriptorPoolCreateInfo pool_info;
    GX_SET_ZERO(pool_info);
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = std::size(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;
    pool_info.maxSets = 1;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT | VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

    GX_VK_CHK(vkCreateDescriptorPool(vk_dev, &pool_info, nullptr, &descriptor_pool));

    VkDescriptorSetAllocateInfo allocate_info;
    GX_SET_ZERO(allocate_info);
    allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocate_info.descriptorPool = descriptor_pool;
    allocate_info.descriptorSetCount = 1;
    allocate_info.pSetLayouts = &set_layout;

    GX_VK_CHK(vkAllocateDescriptorSets(vk_dev, &allocate_info, &static_descriptor_set));

    free_1d_image_indices.reserve(max_1d_images);
    for (std::uint32_t i = max_1d_images; i > 0;) {
         --i;
        free_1d_image_indices.push_back(i);
    }
}

gearoenix::vulkan::descriptor::Bindless::~Bindless()
{
    const auto vk_dev = device::Logical::get().get_vulkan_data();

    if (static_descriptor_set != VK_NULL_HANDLE && descriptor_pool != VK_NULL_HANDLE) {
        vkFreeDescriptorSets(vk_dev, descriptor_pool, 1, &static_descriptor_set);
    }
    
    if (descriptor_pool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(vk_dev, descriptor_pool, nullptr);
    }
    
    if (set_layout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(vk_dev, set_layout, nullptr);
    }
}

VkDescriptorSetLayout gearoenix::vulkan::descriptor::Bindless::get_set_layout() const
{
    return set_layout;
}

VkDescriptorSet gearoenix::vulkan::descriptor::Bindless::get_static_descriptor_set() const
{
    return static_descriptor_set;
}

void gearoenix::vulkan::descriptor::Bindless::write_descriptor(const std::uint32_t index, const VkDescriptorImageInfo& info)
{
    VkWriteDescriptorSet write;
    GX_SET_ZERO(write);
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = static_descriptor_set;
    write.dstBinding = 0;
    write.dstArrayElement = index;
    write.descriptorCount = 1;
    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write.pImageInfo = &info;
    vkUpdateDescriptorSets(device::Logical::get().get_vulkan_data(), 1, &write, 0, nullptr);
}

std::optional<std::uint32_t> gearoenix::vulkan::descriptor::Bindless::allocate_sampled_image(
    const VkImageView view, const VkSampler sampler, const VkImageLayout layout)
{
    const std::lock_guard _lg(sampled_allocation_lock);
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
    const std::lock_guard _lg(sampled_allocation_lock);
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
