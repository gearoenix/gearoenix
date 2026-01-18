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

constexpr std::array gx_static_binding_flags {
    gx_binding_flags, // 1D images
    gx_binding_flags, // 2D images
    gx_binding_flags, // 3D images
    gx_binding_flags, // Cube images
    gx_binding_flags, // Samplers
};
}

gearoenix::vulkan::descriptor::Bindless::Bindless(
    const std::array<std::shared_ptr<buffer::Buffer>, frames_in_flight>& scenes_buffers,
    const std::array<std::shared_ptr<buffer::Buffer>, frames_in_flight>& cameras_buffers,
    const std::array<std::shared_ptr<buffer::Buffer>, frames_in_flight>& models_buffers,
    const std::array<std::shared_ptr<buffer::Buffer>, frames_in_flight>& materials_buffers,
    const std::array<std::shared_ptr<buffer::Buffer>, frames_in_flight>& lights_buffers)
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

    // ========== Static Descriptor Set Layout (Bindless Textures & Samplers) ==========
    std::array<VkDescriptorSetLayoutBinding, gx_static_binding_flags.size()> static_bindings {};
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
    std::array<VkDescriptorSetLayoutBinding, 5> dynamic_bindings {};
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
    std::array<VkDescriptorPoolSize, gx_static_binding_flags.size()> static_pool_sizes{};
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
    std::array<VkDescriptorPoolSize, 1> dynamic_pool_sizes{};
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
    std::array<VkDescriptorSetLayout, frames_in_flight> dynamic_layouts{};
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

    // ========== Initialize Per-Frame Buffer Descriptors ==========
    std::array<VkDescriptorBufferInfo, dynamic_bindings.size()> buffer_infos{};
    std::memset(buffer_infos.data(), 0, sizeof(VkDescriptorBufferInfo) * buffer_infos.size());

    std::array<VkWriteDescriptorSet, dynamic_bindings.size()> writes{};
    std::memset(writes.data(), 0, sizeof(VkWriteDescriptorSet) * writes.size());

    for (std::uint32_t frame_idx = 0; frame_idx < frames_in_flight; ++frame_idx) {
        GX_ASSERT_D(scenes_buffers[frame_idx]);
        GX_ASSERT_D(cameras_buffers[frame_idx]);
        GX_ASSERT_D(models_buffers[frame_idx]);
        GX_ASSERT_D(materials_buffers[frame_idx]);
        GX_ASSERT_D(lights_buffers[frame_idx]);
        
        const std::array buffers = {
            scenes_buffers[frame_idx].get(),
            cameras_buffers[frame_idx].get(),
            models_buffers[frame_idx].get(),
            materials_buffers[frame_idx].get(),
            lights_buffers[frame_idx].get()
        };
        
        for (std::uint32_t i = 0; i < dynamic_bindings.size(); ++i) {
            const auto& buff = *buffers[i];
            const auto& allocator = *buff.get_allocator();
            buffer_infos[i].buffer = buff.get_vulkan_data();
            buffer_infos[i].offset = static_cast<VkDeviceSize>(allocator.get_offset());
            buffer_infos[i].range = static_cast<VkDeviceSize>(allocator.get_size());
            
            writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writes[i].dstSet = dynamic_descriptor_sets[frame_idx];
            writes[i].dstBinding = i;
            writes[i].dstArrayElement = 0;
            writes[i].descriptorCount = 1;
            writes[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            writes[i].pBufferInfo = &buffer_infos[i];
        }

        vkUpdateDescriptorSets(vk_dev, static_cast<std::uint32_t>(writes.size()), writes.data(), 0, nullptr);
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

void gearoenix::vulkan::descriptor::Bindless::write_image_descriptor(const std::uint32_t binding, const std::uint32_t index, const VkDescriptorImageInfo& info) const
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
    
    core::job::send_job(render::engine::Engine::get().get_jobs_thread_id(), [write]() {
        vkUpdateDescriptorSets(device::Logical::get().get_vulkan_data(), 1, &write, 0, nullptr);
    });
}

void gearoenix::vulkan::descriptor::Bindless::write_sampler_descriptor(const std::uint32_t index, const VkDescriptorImageInfo& info) const
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
    
    // Write null descriptor to avoid undefined behavior
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
    
    // Write null descriptor to avoid undefined behavior
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
    
    // Write null descriptor to avoid undefined behavior
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
    
    // Write null descriptor to avoid undefined behavior
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
    
    // Write null descriptor to avoid undefined behavior
    write_sampler_descriptor(index, tracked_info);
    
    free_sampler_indices.push_back(index);
}

#endif
