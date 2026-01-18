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
    , image_1d_infos(max_1d_images)
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
    vkDestroyDescriptorSetLayout(vk_dev, descriptor_set_layout, nullptr);
}

VkDescriptorSetLayout gearoenix::vulkan::descriptor::Bindless::get_descriptor_set_layout() const
{
    return descriptor_set_layout;
}

VkDescriptorSet gearoenix::vulkan::descriptor::Bindless::get_descriptor_set() const
{
    return descriptor_set;
}

void gearoenix::vulkan::descriptor::Bindless::write_image_descriptor(const std::uint32_t binding, const std::uint32_t index, const VkDescriptorImageInfo& info) const
{
    VkWriteDescriptorSet write;
    GX_SET_ZERO(write);
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = descriptor_set;
    write.dstBinding = binding;
    write.dstArrayElement = index;
    write.descriptorCount = 1;
    write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    write.pImageInfo = &info;
    
    core::job::send_job(render::engine::Engine::get().get_jobs_thread_id(), [write]() {
        vkUpdateDescriptorSets(device::Logical::get().get_vulkan_data(), 1, &write, 0, nullptr);
    });
}

void gearoenix::vulkan::descriptor::Bindless::write_sampler_descriptor(const std::uint32_t index, const VkDescriptorImageInfo& info) const
{   
    VkWriteDescriptorSet write;
    GX_SET_ZERO(write);
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = descriptor_set;
    write.dstBinding = 4; // Sampler binding
    write.dstArrayElement = index;
    write.descriptorCount = 1;
    write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
    write.pImageInfo = &info;

    core::job::send_job(render::engine::Engine::get().get_jobs_thread_id(), [write]() {
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

    auto& info = image_1d_infos[index];
    info.imageView = view;
    info.imageLayout = layout;
    write_image_descriptor(0, index, info);
    return index;
}

std::uint32_t gearoenix::vulkan::descriptor::Bindless::allocate_2d_image(const VkImageView view, const VkImageLayout layout)
{
    GX_ASSERT_D(view != VK_NULL_HANDLE);
    const std::lock_guard _lg(allocation_lock);
    GX_ASSERT_D(!free_2d_image_indices.empty());
    const auto index = free_2d_image_indices.back();
    free_2d_image_indices.pop_back();

    auto& info = image_2d_infos[index];
    info.imageView = view;
    info.imageLayout = layout;
    write_image_descriptor(1, index, info);
    return index;
}

std::uint32_t gearoenix::vulkan::descriptor::Bindless::allocate_3d_image(const VkImageView view, const VkImageLayout layout)
{
    GX_ASSERT_D(view != VK_NULL_HANDLE);
    const std::lock_guard _lg(allocation_lock);
    GX_ASSERT_D(!free_3d_image_indices.empty());
    const auto index = free_3d_image_indices.back();
    free_3d_image_indices.pop_back();

    auto& info = image_3d_infos[index];
    info.imageView = view;
    info.imageLayout = layout;
    write_image_descriptor(2, index, info);
    return index;
}

std::uint32_t gearoenix::vulkan::descriptor::Bindless::allocate_cube_image(const VkImageView view, const VkImageLayout layout)
{
    GX_ASSERT_D(view != VK_NULL_HANDLE);
    const std::lock_guard _lg(allocation_lock);
    GX_ASSERT_D(!free_cube_image_indices.empty());
    const auto index = free_cube_image_indices.back();
    free_cube_image_indices.pop_back();

    auto& info = image_cube_infos[index];
    info.imageView = view;
    info.imageLayout = layout;
    write_image_descriptor(3, index, info);
    return index;
}

std::uint32_t gearoenix::vulkan::descriptor::Bindless::allocate_sampler(const VkSampler sampler)
{
    GX_ASSERT_D(sampler != VK_NULL_HANDLE);
    const std::lock_guard _lg(allocation_lock);
    GX_ASSERT_D(!free_sampler_indices.empty());
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
    GX_ASSERT_D(index < image_1d_infos.size());

    auto& tracked_info = image_1d_infos[index];
    GX_ASSERT_D(tracked_info.imageView != VK_NULL_HANDLE);

    tracked_info = {};
    
    // Write a null descriptor to avoid undefined behaviour
    write_image_descriptor(0, index, tracked_info);
    
    free_1d_image_indices.push_back(index);
}

void gearoenix::vulkan::descriptor::Bindless::free_2d_image(const std::uint32_t index)
{
    const std::lock_guard _lg(allocation_lock);
    GX_ASSERT_D(index < image_2d_infos.size());

    auto& tracked_info = image_2d_infos[index];
    GX_ASSERT_D(tracked_info.imageView != VK_NULL_HANDLE);

    tracked_info = {};
    
    // Write a null descriptor to avoid undefined behaviour
    write_image_descriptor(1, index, tracked_info);
    
    free_2d_image_indices.push_back(index);
}

void gearoenix::vulkan::descriptor::Bindless::free_3d_image(const std::uint32_t index)
{
    const std::lock_guard _lg(allocation_lock);
    GX_ASSERT_D(index < image_3d_infos.size());

    auto& tracked_info = image_3d_infos[index];
    GX_ASSERT_D(tracked_info.imageView != VK_NULL_HANDLE);

    tracked_info = {};
    
    // Write a null descriptor to avoid undefined behaviour
    write_image_descriptor(2, index, tracked_info);
    
    free_3d_image_indices.push_back(index);
}

void gearoenix::vulkan::descriptor::Bindless::free_cube_image(const std::uint32_t index)
{
    const std::lock_guard _lg(allocation_lock);
    GX_ASSERT_D(index < image_cube_infos.size());

    auto& tracked_info = image_cube_infos[index];
    GX_ASSERT_D(tracked_info.imageView != VK_NULL_HANDLE);

    tracked_info = {};
    
    // Write a null descriptor to avoid undefined behaviour
    write_image_descriptor(3, index, tracked_info);
    
    free_cube_image_indices.push_back(index);
}

void gearoenix::vulkan::descriptor::Bindless::free_sampler(const std::uint32_t index)
{
    const std::lock_guard _lg(allocation_lock);
    GX_ASSERT_D(index < sampler_infos.size());

    auto& tracked_info = sampler_infos[index];
    GX_ASSERT_D(tracked_info.sampler != VK_NULL_HANDLE);

    tracked_info = {};
    
    // Write a null descriptor to avoid undefined behaviour
    write_sampler_descriptor(index, tracked_info);
    
    free_sampler_indices.push_back(index);
}

#endif
