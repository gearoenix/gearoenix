#include "gx-vk-des-bindless.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../gx-vk-check.hpp"

#include <array>

namespace {
constexpr VkDescriptorBindingFlags gx_binding_flags =
    VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT |
    VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT |
    VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT;

constexpr std::array gx_static_binding_flags {
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
    std::memset(image_1d_infos.data(), 0, sizeof(VkDescriptorImageInfo) * image_1d_infos.size());
    std::memset(image_2d_infos.data(), 0, sizeof(VkDescriptorImageInfo) * image_2d_infos.size());
    std::memset(image_3d_infos.data(), 0, sizeof(VkDescriptorImageInfo) * image_3d_infos.size());
    std::memset(image_cube_infos.data(), 0, sizeof(VkDescriptorImageInfo) * image_cube_infos.size());
    std::memset(sampler_infos.data(), 0, sizeof(VkDescriptorImageInfo) * sampler_infos.size());

    const auto vk_dev = device::Logical::get().get_vulkan_data();

    // ========== Static Descriptor Set Layout (Bindless Textures & Samplers) ==========
    std::array<VkDescriptorSetLayoutBinding, gx_static_binding_flags.size()> static_bindings;
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

    VkDescriptorSetLayoutBindingFlagsCreateInfo static_binding_flags_info;
    GX_SET_ZERO(static_binding_flags_info);
    static_binding_flags_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
    static_binding_flags_info.bindingCount = static_cast<std::uint32_t>(gx_static_binding_flags.size());
    static_binding_flags_info.pBindingFlags = gx_static_binding_flags.data();

    VkDescriptorSetLayoutCreateInfo static_layout_info;
    GX_SET_ZERO(static_layout_info);
    static_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    static_layout_info.bindingCount = static_cast<std::uint32_t>(static_bindings.size());
    static_layout_info.pBindings = static_bindings.data();
    static_layout_info.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
    static_layout_info.pNext = &static_binding_flags_info;

    GX_VK_CHK(vkCreateDescriptorSetLayout(vk_dev, &static_layout_info, nullptr, &static_set_layout));

    // ========== Dynamic Descriptor Set Layout (Per-Frame Storage Buffers) ==========
    std::array<VkDescriptorSetLayoutBinding, 5> dynamic_bindings;
    GX_SET_ZERO(dynamic_bindings);

    dynamic_bindings[0].binding = 0;
    dynamic_bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    dynamic_bindings[0].descriptorCount = 1;
    dynamic_bindings[0].stageFlags = VK_SHADER_STAGE_ALL;

    dynamic_bindings[1].binding = 1;
    dynamic_bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    dynamic_bindings[1].descriptorCount = 1;
    dynamic_bindings[1].stageFlags = VK_SHADER_STAGE_ALL;

    dynamic_bindings[2].binding = 2;
    dynamic_bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    dynamic_bindings[2].descriptorCount = 1;
    dynamic_bindings[2].stageFlags = VK_SHADER_STAGE_ALL;

    dynamic_bindings[3].binding = 3;
    dynamic_bindings[3].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    dynamic_bindings[3].descriptorCount = 1;
    dynamic_bindings[3].stageFlags = VK_SHADER_STAGE_ALL;

    dynamic_bindings[4].binding = 4;
    dynamic_bindings[4].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    dynamic_bindings[4].descriptorCount = 1;
    dynamic_bindings[4].stageFlags = VK_SHADER_STAGE_ALL;

    VkDescriptorSetLayoutCreateInfo dynamic_layout_info;
    GX_SET_ZERO(dynamic_layout_info);
    dynamic_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    dynamic_layout_info.bindingCount = static_cast<std::uint32_t>(dynamic_bindings.size());
    dynamic_layout_info.pBindings = dynamic_bindings.data();

    GX_VK_CHK(vkCreateDescriptorSetLayout(vk_dev, &dynamic_layout_info, nullptr, &dynamic_set_layout));

    // ========== Static Descriptor Pool ==========
    std::array<VkDescriptorPoolSize, gx_static_binding_flags.size()> static_pool_sizes;
    GX_SET_ZERO(static_pool_sizes);
    
    static_pool_sizes[0].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    static_pool_sizes[0].descriptorCount = max_1d_images;

    static_pool_sizes[1].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    static_pool_sizes[1].descriptorCount = max_2d_images;

    static_pool_sizes[2].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    static_pool_sizes[2].descriptorCount = max_3d_images;

    static_pool_sizes[3].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    static_pool_sizes[3].descriptorCount = max_cube_images;
    
    static_pool_sizes[4].type = VK_DESCRIPTOR_TYPE_SAMPLER;
    static_pool_sizes[4].descriptorCount = max_samplers;

    VkDescriptorPoolCreateInfo static_pool_info;
    GX_SET_ZERO(static_pool_info);
    static_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    static_pool_info.poolSizeCount = static_cast<std::uint32_t>(static_pool_sizes.size());
    static_pool_info.pPoolSizes = static_pool_sizes.data();
    static_pool_info.maxSets = 1;
    static_pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT | VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

    GX_VK_CHK(vkCreateDescriptorPool(vk_dev, &static_pool_info, nullptr, &static_descriptor_pool));

    // ========== Dynamic Descriptor Pool ==========
    std::array<VkDescriptorPoolSize, 1> dynamic_pool_sizes;
    GX_SET_ZERO(dynamic_pool_sizes);
    
    dynamic_pool_sizes[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    dynamic_pool_sizes[0].descriptorCount = static_cast<std::uint32_t>(dynamic_bindings.size()) * frames_in_flight; // 5 buffers per frame

    VkDescriptorPoolCreateInfo dynamic_pool_info;
    GX_SET_ZERO(dynamic_pool_info);
    dynamic_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    dynamic_pool_info.poolSizeCount = static_cast<std::uint32_t>(dynamic_pool_sizes.size());
    dynamic_pool_info.pPoolSizes = dynamic_pool_sizes.data();
    dynamic_pool_info.maxSets = frames_in_flight;

    GX_VK_CHK(vkCreateDescriptorPool(vk_dev, &dynamic_pool_info, nullptr, &dynamic_descriptor_pool));

    // ========== Allocate Static Descriptor Set ==========
    VkDescriptorSetAllocateInfo static_allocate_info;
    GX_SET_ZERO(static_allocate_info);
    static_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    static_allocate_info.descriptorPool = static_descriptor_pool;
    static_allocate_info.descriptorSetCount = 1;
    static_allocate_info.pSetLayouts = &static_set_layout;

    GX_VK_CHK(vkAllocateDescriptorSets(vk_dev, &static_allocate_info, &static_descriptor_set));

    // ========== Allocate Dynamic Descriptor Sets (Per Frame) ==========
    std::array<VkDescriptorSetLayout, frames_in_flight> dynamic_layouts;
    for (auto& layout : dynamic_layouts) {
        layout = dynamic_set_layout;
    }

    VkDescriptorSetAllocateInfo dynamic_allocate_info;
    GX_SET_ZERO(dynamic_allocate_info);
    dynamic_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    dynamic_allocate_info.descriptorPool = dynamic_descriptor_pool;
    dynamic_allocate_info.descriptorSetCount = static_cast<std::uint32_t>(dynamic_layouts.size());
    dynamic_allocate_info.pSetLayouts = dynamic_layouts.data();

    GX_VK_CHK(vkAllocateDescriptorSets(vk_dev, &dynamic_allocate_info, dynamic_descriptor_sets.data()));

    // ========== Initialize Free Index Lists ==========
    free_1d_image_indices.reserve(max_1d_images);
    for (std::uint32_t i = max_1d_images; i > 0;) {
        --i;
        free_1d_image_indices.push_back(i);
    }

    free_2d_image_indices.reserve(max_2d_images);
    for (std::uint32_t i = max_2d_images; i > 0;) {
        --i;
        free_2d_image_indices.push_back(i);
    }

    free_3d_image_indices.reserve(max_3d_images);
    for (std::uint32_t i = max_3d_images; i > 0;) {
        --i;
        free_3d_image_indices.push_back(i);
    }

    free_cube_image_indices.reserve(max_cube_images);
    for (std::uint32_t i = max_cube_images; i > 0;) {
        --i;
        free_cube_image_indices.push_back(i);
    }

    free_sampler_indices.reserve(max_samplers);
    for (std::uint32_t i = max_samplers; i > 0;) {
        --i;
        free_sampler_indices.push_back(i);
    }
}

gearoenix::vulkan::descriptor::Bindless::~Bindless()
{
    const auto vk_dev = device::Logical::get().get_vulkan_data();

    if (!dynamic_descriptor_sets.empty() && dynamic_descriptor_pool != VK_NULL_HANDLE) {
        vkFreeDescriptorSets(vk_dev, dynamic_descriptor_pool, static_cast<std::uint32_t>(dynamic_descriptor_sets.size()), dynamic_descriptor_sets.data());
    }

    if (static_descriptor_set != VK_NULL_HANDLE && static_descriptor_pool != VK_NULL_HANDLE) {
        vkFreeDescriptorSets(vk_dev, static_descriptor_pool, 1, &static_descriptor_set);
    }
    
    if (dynamic_descriptor_pool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(vk_dev, dynamic_descriptor_pool, nullptr);
    }

    if (static_descriptor_pool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(vk_dev, static_descriptor_pool, nullptr);
    }
    
    if (dynamic_set_layout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(vk_dev, dynamic_set_layout, nullptr);
    }

    if (static_set_layout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(vk_dev, static_set_layout, nullptr);
    }
}

VkDescriptorSetLayout gearoenix::vulkan::descriptor::Bindless::get_static_set_layout() const
{
    return static_set_layout;
}

VkDescriptorSetLayout gearoenix::vulkan::descriptor::Bindless::get_dynamic_set_layout() const
{
    return dynamic_set_layout;
}

VkDescriptorSet gearoenix::vulkan::descriptor::Bindless::get_static_descriptor_set() const
{
    return static_descriptor_set;
}

VkDescriptorSet gearoenix::vulkan::descriptor::Bindless::get_dynamic_descriptor_set(const std::uint32_t frame_index) const
{
    GX_ASSERT_D(frame_index < frames_in_flight);
    return dynamic_descriptor_sets[frame_index];
}

void gearoenix::vulkan::descriptor::Bindless::write_image_descriptor(const std::uint32_t binding, const std::uint32_t index, const VkDescriptorImageInfo& info)
{
    VkWriteDescriptorSet write;
    GX_SET_ZERO(write);
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = static_descriptor_set;
    write.dstBinding = binding;
    write.dstArrayElement = index;
    write.descriptorCount = 1;
    write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    write.pImageInfo = &info;
    vkUpdateDescriptorSets(device::Logical::get().get_vulkan_data(), 1, &write, 0, nullptr);
}

void gearoenix::vulkan::descriptor::Bindless::write_sampler_descriptor(const std::uint32_t index, const VkDescriptorImageInfo& info)
{
    VkWriteDescriptorSet write;
    GX_SET_ZERO(write);
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = static_descriptor_set;
    write.dstBinding = 4; // Sampler binding
    write.dstArrayElement = index;
    write.descriptorCount = 1;
    write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
    write.pImageInfo = &info;
    vkUpdateDescriptorSets(device::Logical::get().get_vulkan_data(), 1, &write, 0, nullptr);
}

std::optional<std::uint32_t> gearoenix::vulkan::descriptor::Bindless::allocate_1d_image(const VkImageView view, const VkImageLayout layout)
{
    const std::lock_guard _lg(allocation_lock);
    if (free_1d_image_indices.empty()) {
        return std::nullopt;
    }
    const auto index = free_1d_image_indices.back();
    free_1d_image_indices.pop_back();

    auto& info = image_1d_infos[index];
    info.imageView = view;
    info.imageLayout = layout;
    write_image_descriptor(0, index, info);
    return index;
}

std::optional<std::uint32_t> gearoenix::vulkan::descriptor::Bindless::allocate_2d_image(const VkImageView view, const VkImageLayout layout)
{
    const std::lock_guard _lg(allocation_lock);
    if (free_2d_image_indices.empty()) {
        return std::nullopt;
    }
    const auto index = free_2d_image_indices.back();
    free_2d_image_indices.pop_back();

    auto& info = image_2d_infos[index];
    info.imageView = view;
    info.imageLayout = layout;
    write_image_descriptor(1, index, info);
    return index;
}

std::optional<std::uint32_t> gearoenix::vulkan::descriptor::Bindless::allocate_3d_image(const VkImageView view, const VkImageLayout layout)
{
    const std::lock_guard _lg(allocation_lock);
    if (free_3d_image_indices.empty()) {
        return std::nullopt;
    }
    const auto index = free_3d_image_indices.back();
    free_3d_image_indices.pop_back();

    auto& info = image_3d_infos[index];
    info.imageView = view;
    info.imageLayout = layout;
    write_image_descriptor(2, index, info);
    return index;
}

std::optional<std::uint32_t> gearoenix::vulkan::descriptor::Bindless::allocate_cube_image(const VkImageView view, const VkImageLayout layout)
{
    const std::lock_guard _lg(allocation_lock);
    if (free_cube_image_indices.empty()) {
        return std::nullopt;
    }
    const auto index = free_cube_image_indices.back();
    free_cube_image_indices.pop_back();

    auto& info = image_cube_infos[index];
    info.imageView = view;
    info.imageLayout = layout;
    write_image_descriptor(3, index, info);
    return index;
}

std::optional<std::uint32_t> gearoenix::vulkan::descriptor::Bindless::allocate_sampler(const VkSampler sampler)
{
    const std::lock_guard _lg(allocation_lock);
    if (free_sampler_indices.empty()) {
        return std::nullopt;
    }
    const auto index = free_sampler_indices.back();
    free_sampler_indices.pop_back();

    auto& info = sampler_infos[index];
    info.sampler = sampler;
    write_sampler_descriptor(index, info);
    return index;
}

void gearoenix::vulkan::descriptor::Bindless::free_1d_image(const std::uint32_t index)
{
    const std::lock_guard _lg(allocation_lock);
    if (index >= image_1d_infos.size()) {
        return;
    }

    auto& tracked_info = image_1d_infos[index];
    if (tracked_info.imageView == VK_NULL_HANDLE) {
        return;
    }

    tracked_info = {};
    free_1d_image_indices.push_back(index);
}

void gearoenix::vulkan::descriptor::Bindless::free_2d_image(const std::uint32_t index)
{
    const std::lock_guard _lg(allocation_lock);
    if (index >= image_2d_infos.size()) {
        return;
    }

    auto& tracked_info = image_2d_infos[index];
    if (tracked_info.imageView == VK_NULL_HANDLE) {
        return;
    }

    tracked_info = {};
    free_2d_image_indices.push_back(index);
}

void gearoenix::vulkan::descriptor::Bindless::free_3d_image(const std::uint32_t index)
{
    const std::lock_guard _lg(allocation_lock);
    if (index >= image_3d_infos.size()) {
        return;
    }

    auto& tracked_info = image_3d_infos[index];
    if (tracked_info.imageView == VK_NULL_HANDLE) {
        return;
    }

    tracked_info = {};
    free_3d_image_indices.push_back(index);
}

void gearoenix::vulkan::descriptor::Bindless::free_cube_image(const std::uint32_t index)
{
    const std::lock_guard _lg(allocation_lock);
    if (index >= image_cube_infos.size()) {
        return;
    }

    auto& tracked_info = image_cube_infos[index];
    if (tracked_info.imageView == VK_NULL_HANDLE) {
        return;
    }

    tracked_info = {};
    free_cube_image_indices.push_back(index);
}

void gearoenix::vulkan::descriptor::Bindless::free_sampler(const std::uint32_t index)
{
    const std::lock_guard _lg(allocation_lock);
    if (index >= sampler_infos.size()) {
        return;
    }

    auto& tracked_info = sampler_infos[index];
    if (tracked_info.sampler == VK_NULL_HANDLE) {
        return;
    }

    tracked_info = {};
    free_sampler_indices.push_back(index);
}

void gearoenix::vulkan::descriptor::Bindless::update_dynamic_buffers(
    const std::uint32_t frame_index,
    const VkBuffer scenes_buffer,
    const VkBuffer cameras_buffer,
    const VkBuffer models_buffer,
    const VkBuffer materials_buffer,
    const VkBuffer lights_buffer)
{
    GX_ASSERT_D(frame_index < frames_in_flight);

    VkDescriptorBufferInfo buffer_infos[5];
    GX_SET_ZERO(buffer_infos);

    buffer_infos[0].buffer = scenes_buffer;
    buffer_infos[0].offset = 0;
    buffer_infos[0].range = VK_WHOLE_SIZE;

    buffer_infos[1].buffer = cameras_buffer;
    buffer_infos[1].offset = 0;
    buffer_infos[1].range = VK_WHOLE_SIZE;

    buffer_infos[2].buffer = models_buffer;
    buffer_infos[2].offset = 0;
    buffer_infos[2].range = VK_WHOLE_SIZE;

    buffer_infos[3].buffer = materials_buffer;
    buffer_infos[3].offset = 0;
    buffer_infos[3].range = VK_WHOLE_SIZE;

    buffer_infos[4].buffer = lights_buffer;
    buffer_infos[4].offset = 0;
    buffer_infos[4].range = VK_WHOLE_SIZE;

    VkWriteDescriptorSet writes[5];
    GX_SET_ZERO(writes);

    for (std::uint32_t i = 0; i < 5; ++i) {
        writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes[i].dstSet = dynamic_descriptor_sets[frame_index];
        writes[i].dstBinding = i;
        writes[i].dstArrayElement = 0;
        writes[i].descriptorCount = 1;
        writes[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        writes[i].pBufferInfo = &buffer_infos[i];
    }

    vkUpdateDescriptorSets(device::Logical::get().get_vulkan_data(), 5, writes, 0, nullptr);
}

#endif
