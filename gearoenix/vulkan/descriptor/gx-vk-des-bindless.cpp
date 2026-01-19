#include "gx-vk-des-bindless.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/job/gx-cr-job-manager.hpp"
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../../core/allocator/gx-cr-alc-range.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../buffer/gx-vk-buf-buffer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../gx-vk-check.hpp"

namespace {
constexpr VkDescriptorBindingFlags gx_binding_flags =
    VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT |
    VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT |
    VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT;
}

gearoenix::vulkan::descriptor::Bindless::Bindless(
    const buffer::Buffer& scenes_buffer,
    const buffer::Buffer& cameras_buffer,
    const buffer::Buffer& models_buffer,
    const buffer::Buffer& materials_buffer,
    const buffer::Buffer& lights_buffer)
    : Singleton(this)
{
    const auto vk_dev = device::Logical::get().get_vulkan_data();

    // ========== Descriptor Set Layout (Bindless Textures, Samplers & Buffers) ==========
    std::array<VkDescriptorSetLayoutBinding, 10> bindings {};
    GX_SET_ZERO(bindings);

    // Bindless texture bindings
    bindings[0].binding = 0;
    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    bindings[0].descriptorCount = max_1d_images;
    bindings[0].stageFlags = VK_SHADER_STAGE_ALL;

    bindings[1].binding = 1;
    bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    bindings[1].descriptorCount = max_2d_images;
    bindings[1].stageFlags = VK_SHADER_STAGE_ALL;

    bindings[2].binding = 2;
    bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    bindings[2].descriptorCount = max_3d_images;
    bindings[2].stageFlags = VK_SHADER_STAGE_ALL;

    bindings[3].binding = 3;
    bindings[3].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    bindings[3].descriptorCount = max_cube_images;
    bindings[3].stageFlags = VK_SHADER_STAGE_ALL;

    bindings[4].binding = 4;
    bindings[4].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
    bindings[4].descriptorCount = max_samplers;
    bindings[4].stageFlags = VK_SHADER_STAGE_ALL;

    // Storage buffer bindings
    bindings[5].binding = 5;
    bindings[5].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    bindings[5].descriptorCount = 1;
    bindings[5].stageFlags = VK_SHADER_STAGE_ALL;

    bindings[6].binding = 6;
    bindings[6].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    bindings[6].descriptorCount = 1;
    bindings[6].stageFlags = VK_SHADER_STAGE_ALL;

    bindings[7].binding = 7;
    bindings[7].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    bindings[7].descriptorCount = 1;
    bindings[7].stageFlags = VK_SHADER_STAGE_ALL;

    bindings[8].binding = 8;
    bindings[8].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    bindings[8].descriptorCount = 1;
    bindings[8].stageFlags = VK_SHADER_STAGE_ALL;

    bindings[9].binding = 9;
    bindings[9].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    bindings[9].descriptorCount = 1;
    bindings[9].stageFlags = VK_SHADER_STAGE_ALL;

    std::array<VkDescriptorBindingFlags, 10> binding_flags {
        gx_binding_flags, // 1D images
        gx_binding_flags, // 2D images
        gx_binding_flags, // 3D images
        gx_binding_flags, // Cube images
        gx_binding_flags, // Samplers
        0, // scenes buffer
        0, // cameras buffer
        0, // models buffer
        0, // materials buffer
        0  // lights buffer
    };

    VkDescriptorSetLayoutBindingFlagsCreateInfo binding_flags_info;
    GX_SET_ZERO(binding_flags_info);
    binding_flags_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
    binding_flags_info.bindingCount = static_cast<std::uint32_t>(binding_flags.size());
    binding_flags_info.pBindingFlags = binding_flags.data();

    VkDescriptorSetLayoutCreateInfo layout_info;
    GX_SET_ZERO(layout_info);
    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.bindingCount = static_cast<std::uint32_t>(bindings.size());
    layout_info.pBindings = bindings.data();
    layout_info.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
    layout_info.pNext = &binding_flags_info;

    GX_VK_CHK(vkCreateDescriptorSetLayout(vk_dev, &layout_info, nullptr, &descriptor_set_layout));

    // ========== Pipeline Layout ==========
    VkPushConstantRange push_constant_range;
    GX_SET_ZERO(push_constant_range);
    push_constant_range.stageFlags = VK_SHADER_STAGE_ALL;
    push_constant_range.offset = 0;
    push_constant_range.size = sizeof(BindlessPushConstants);

    VkPipelineLayoutCreateInfo pipeline_layout_info;
    GX_SET_ZERO(pipeline_layout_info);
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = 1;
    pipeline_layout_info.pSetLayouts = &descriptor_set_layout;
    pipeline_layout_info.pushConstantRangeCount = 1;
    pipeline_layout_info.pPushConstantRanges = &push_constant_range;

    GX_VK_CHK(vkCreatePipelineLayout(vk_dev, &pipeline_layout_info, nullptr, &pipeline_layout));

    // ========== Descriptor Pool ==========
    std::array<VkDescriptorPoolSize, 6> pool_sizes{};
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

    pool_sizes[5].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    pool_sizes[5].descriptorCount = 5; // 5 storage buffers

    VkDescriptorPoolCreateInfo pool_info;
    GX_SET_ZERO(pool_info);
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = static_cast<std::uint32_t>(pool_sizes.size());
    pool_info.pPoolSizes = pool_sizes.data();
    pool_info.maxSets = 1;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT | VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

    GX_VK_CHK(vkCreateDescriptorPool(vk_dev, &pool_info, nullptr, &descriptor_pool));

    // ========== Allocate Descriptor Set ==========
    VkDescriptorSetAllocateInfo allocate_info;
    GX_SET_ZERO(allocate_info);
    allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocate_info.descriptorPool = descriptor_pool;
    allocate_info.descriptorSetCount = 1;
    allocate_info.pSetLayouts = &descriptor_set_layout;

    GX_VK_CHK(vkAllocateDescriptorSets(vk_dev, &allocate_info, &descriptor_set));

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

    // ========== Initialize Buffer Descriptors ==========    
    const std::array buffers = {
        &scenes_buffer,
        &cameras_buffer,
        &models_buffer,
        &materials_buffer,
        &lights_buffer
    };

    std::array<VkDescriptorBufferInfo, 5> buffer_infos{};
    std::memset(buffer_infos.data(), 0, sizeof(VkDescriptorBufferInfo) * buffer_infos.size());

    std::array<VkWriteDescriptorSet, 5> writes{};
    std::memset(writes.data(), 0, sizeof(VkWriteDescriptorSet) * writes.size());
    
    for (std::uint32_t i = 0; i < buffers.size(); ++i) {
        const auto& buff = *buffers[i];
        const auto& allocator = *buff.get_allocator();
        buffer_infos[i].buffer = buff.get_vulkan_data();
        buffer_infos[i].offset = static_cast<VkDeviceSize>(allocator.get_offset());
        buffer_infos[i].range = static_cast<VkDeviceSize>(allocator.get_size());
        
        writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes[i].dstSet = descriptor_set;
        writes[i].dstBinding = 5 + i; // Buffers start at binding 5
        writes[i].dstArrayElement = 0;
        writes[i].descriptorCount = 1;
        writes[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        writes[i].pBufferInfo = &buffer_infos[i];
    }

    vkUpdateDescriptorSets(vk_dev, static_cast<std::uint32_t>(writes.size()), writes.data(), 0, nullptr);
}

gearoenix::vulkan::descriptor::Bindless::~Bindless()
{
    const auto vk_dev = device::Logical::get().get_vulkan_data();

    vkFreeDescriptorSets(vk_dev, descriptor_pool, 1, &descriptor_set);
    vkDestroyDescriptorPool(vk_dev, descriptor_pool, nullptr);
    vkDestroyPipelineLayout(vk_dev, pipeline_layout, nullptr);
    vkDestroyDescriptorSetLayout(vk_dev, descriptor_set_layout, nullptr);
}

void gearoenix::vulkan::descriptor::Bindless::write_image_descriptor(const std::uint32_t binding, const std::uint32_t index, const VkImageView view, const VkImageLayout layout) const
{
    VkDescriptorImageInfo info;
    GX_SET_ZERO(info);
    info.imageView = view;
    info.imageLayout = layout;
    
    VkWriteDescriptorSet write;
    GX_SET_ZERO(write);
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = descriptor_set;
    write.dstBinding = binding;
    write.dstArrayElement = index;
    write.descriptorCount = 1;
    write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    
    core::job::send_job(render::engine::Engine::get().get_jobs_thread_id(), [write, info]() mutable {
        write.pImageInfo = &info;
        vkUpdateDescriptorSets(device::Logical::get().get_vulkan_data(), 1, &write, 0, nullptr);
    });
}

void gearoenix::vulkan::descriptor::Bindless::write_sampler_descriptor(const std::uint32_t index, const VkSampler sampler) const
{   
    VkDescriptorImageInfo info;
    GX_SET_ZERO(info);
    info.sampler = sampler;
    
    VkWriteDescriptorSet write;
    GX_SET_ZERO(write);
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = descriptor_set;
    write.dstBinding = 4; // Sampler binding
    write.dstArrayElement = index;
    write.descriptorCount = 1;
    write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;

    core::job::send_job(render::engine::Engine::get().get_jobs_thread_id(), [write, info]() mutable {
        write.pImageInfo = &info;
        vkUpdateDescriptorSets(device::Logical::get().get_vulkan_data(), 1, &write, 0, nullptr);
    });
}

std::uint32_t gearoenix::vulkan::descriptor::Bindless::allocate_1d_image(const VkImageView view, const VkImageLayout layout)
{
    GX_ASSERT_D(view != VK_NULL_HANDLE);
    const std::lock_guard _lg(allocation_lock);
    GX_ASSERT_D(!free_1d_image_indices.empty());
    const auto index = free_1d_image_indices.back();
    free_1d_image_indices.pop_back();

    write_image_descriptor(0, index, view, layout);
    return index;
}

std::uint32_t gearoenix::vulkan::descriptor::Bindless::allocate_2d_image(const VkImageView view, const VkImageLayout layout)
{
    GX_ASSERT_D(view != VK_NULL_HANDLE);
    const std::lock_guard _lg(allocation_lock);
    GX_ASSERT_D(!free_2d_image_indices.empty());
    const auto index = free_2d_image_indices.back();
    free_2d_image_indices.pop_back();

    write_image_descriptor(1, index, view, layout);
    return index;
}

std::uint32_t gearoenix::vulkan::descriptor::Bindless::allocate_3d_image(const VkImageView view, const VkImageLayout layout)
{
    GX_ASSERT_D(view != VK_NULL_HANDLE);
    const std::lock_guard _lg(allocation_lock);
    GX_ASSERT_D(!free_3d_image_indices.empty());
    const auto index = free_3d_image_indices.back();
    free_3d_image_indices.pop_back();

    write_image_descriptor(2, index, view, layout);
    return index;
}

std::uint32_t gearoenix::vulkan::descriptor::Bindless::allocate_cube_image(const VkImageView view, const VkImageLayout layout)
{
    GX_ASSERT_D(view != VK_NULL_HANDLE);
    const std::lock_guard _lg(allocation_lock);
    GX_ASSERT_D(!free_cube_image_indices.empty());
    const auto index = free_cube_image_indices.back();
    free_cube_image_indices.pop_back();

    write_image_descriptor(3, index, view, layout);
    return index;
}

std::uint32_t gearoenix::vulkan::descriptor::Bindless::allocate_sampler(const VkSampler sampler)
{
    GX_ASSERT_D(sampler != VK_NULL_HANDLE);
    const std::lock_guard _lg(allocation_lock);
    GX_ASSERT_D(!free_sampler_indices.empty());
    const auto index = free_sampler_indices.back();
    free_sampler_indices.pop_back();

    write_sampler_descriptor(index, sampler);
    return index;
}

void gearoenix::vulkan::descriptor::Bindless::free_1d_image(const std::uint32_t index)
{
    const std::lock_guard _lg(allocation_lock);
    GX_ASSERT_D(index < max_1d_images);
    
    // Write a null descriptor to avoid undefined behaviour
    write_image_descriptor(0, index, VK_NULL_HANDLE, VK_IMAGE_LAYOUT_UNDEFINED);
    
    free_1d_image_indices.push_back(index);
}

void gearoenix::vulkan::descriptor::Bindless::free_2d_image(const std::uint32_t index)
{
    const std::lock_guard _lg(allocation_lock);
    GX_ASSERT_D(index < max_2d_images);
    
    // Write a null descriptor to avoid undefined behaviour
    write_image_descriptor(1, index, VK_NULL_HANDLE, VK_IMAGE_LAYOUT_UNDEFINED);
    
    free_2d_image_indices.push_back(index);
}

void gearoenix::vulkan::descriptor::Bindless::free_3d_image(const std::uint32_t index)
{
    const std::lock_guard _lg(allocation_lock);
    GX_ASSERT_D(index < max_3d_images);
    
    // Write a null descriptor to avoid undefined behaviour
    write_image_descriptor(2, index, VK_NULL_HANDLE, VK_IMAGE_LAYOUT_UNDEFINED);
    
    free_3d_image_indices.push_back(index);
}

void gearoenix::vulkan::descriptor::Bindless::free_cube_image(const std::uint32_t index)
{
    const std::lock_guard _lg(allocation_lock);
    GX_ASSERT_D(index < max_cube_images);
    
    // Write a null descriptor to avoid undefined behaviour
    write_image_descriptor(3, index, VK_NULL_HANDLE, VK_IMAGE_LAYOUT_UNDEFINED);
    
    free_cube_image_indices.push_back(index);
}

void gearoenix::vulkan::descriptor::Bindless::free_sampler(const std::uint32_t index)
{
    const std::lock_guard _lg(allocation_lock);
    GX_ASSERT_D(index < max_samplers);
    
    // Write a null descriptor to avoid undefined behaviour
    write_sampler_descriptor(index, VK_NULL_HANDLE);
    
    free_sampler_indices.push_back(index);
}

void gearoenix::vulkan::descriptor::Bindless::bind(const VkCommandBuffer cmd) const
{
    vkCmdBindDescriptorSets(cmd,VK_PIPELINE_BIND_POINT_GRAPHICS,pipeline_layout, 0, 1, &descriptor_set, 0, nullptr);
}

#endif
