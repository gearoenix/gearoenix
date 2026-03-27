#include "gx-vk-des-bindless.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/allocator/gx-cr-alc-range.hpp"
#include "../../core/job/gx-cr-job-manager.hpp"
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../buffer/gx-vk-buf-buffer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../memory/gx-vk-mem-memory.hpp"
#include "../pipeline/gx-vk-pip-push-constant.hpp"

namespace {
constexpr vk::DescriptorBindingFlags gx_binding_flags = vk::DescriptorBindingFlagBits::ePartiallyBound | vk::DescriptorBindingFlagBits::eUpdateAfterBind | vk::DescriptorBindingFlagBits::eUpdateUnusedWhilePending;
}

gearoenix::vulkan::descriptor::Bindless::Bindless(
    const buffer::Buffer& scenes_buffer,
    const buffer::Buffer& cameras_buffer,
    const buffer::Buffer& models_buffer,
    const buffer::Buffer& materials_buffer,
    const buffer::Buffer& point_lights_buffer,
    const buffer::Buffer& directional_lights_buffer,
    const buffer::Buffer& shadow_caster_directional_lights_buffer,
    const buffer::Buffer& bones_buffer,
    const buffer::Buffer& reflection_probes_buffer,
    const buffer::Buffer& cameras_joint_models_buffer)
    : Singleton(this)
{
    const auto& dev_raii = device::Logical::get().get_device();
    const auto dev = device::Logical::get().get_vulkan_data();

    // ========== Descriptor Set Layout (Bindless Textures, Samplers & Buffers) ==========
    constexpr auto all = vk::ShaderStageFlagBits::eAll;
    constexpr std::array bindings {
        vk::DescriptorSetLayoutBinding { 0, vk::DescriptorType::eSampledImage, max_1d_images, all },
        vk::DescriptorSetLayoutBinding { 1, vk::DescriptorType::eSampledImage, max_2d_images, all },
        vk::DescriptorSetLayoutBinding { 2, vk::DescriptorType::eSampledImage, max_3d_images, all },
        vk::DescriptorSetLayoutBinding { 3, vk::DescriptorType::eSampledImage, max_cube_images, all },
        vk::DescriptorSetLayoutBinding { 4, vk::DescriptorType::eSampledImage, max_shadow_2d_images, all },
        vk::DescriptorSetLayoutBinding { 5, vk::DescriptorType::eSampler, max_samplers, all },
        vk::DescriptorSetLayoutBinding { 6, vk::DescriptorType::eSampler, max_shadow_samplers, all },
        vk::DescriptorSetLayoutBinding { 7, vk::DescriptorType::eStorageBuffer, 1, all },
        vk::DescriptorSetLayoutBinding { 8, vk::DescriptorType::eStorageBuffer, 1, all },
        vk::DescriptorSetLayoutBinding { 9, vk::DescriptorType::eStorageBuffer, 1, all },
        vk::DescriptorSetLayoutBinding { 10, vk::DescriptorType::eStorageBuffer, 1, all },
        vk::DescriptorSetLayoutBinding { 11, vk::DescriptorType::eStorageBuffer, 1, all },
        vk::DescriptorSetLayoutBinding { 12, vk::DescriptorType::eStorageBuffer, 1, all },
        vk::DescriptorSetLayoutBinding { 13, vk::DescriptorType::eStorageBuffer, 1, all },
        vk::DescriptorSetLayoutBinding { 14, vk::DescriptorType::eStorageBuffer, 1, all },
        vk::DescriptorSetLayoutBinding { 15, vk::DescriptorType::eStorageBuffer, 1, all },
        vk::DescriptorSetLayoutBinding { 16, vk::DescriptorType::eStorageBuffer, 1, all },
    };

    constexpr std::array binding_flags {
        gx_binding_flags, // 1D images
        gx_binding_flags, // 2D images
        gx_binding_flags, // 3D images
        gx_binding_flags, // Cube images
        gx_binding_flags, // shadow/depth 2D images
        gx_binding_flags, // Samplers
        vk::DescriptorBindingFlags { }, // shadow comparison sampler
        vk::DescriptorBindingFlags { }, // scenes buffer
        vk::DescriptorBindingFlags { }, // cameras buffer
        vk::DescriptorBindingFlags { }, // models buffer
        vk::DescriptorBindingFlags { }, // materials buffer
        vk::DescriptorBindingFlags { }, // point lights buffer
        vk::DescriptorBindingFlags { }, // directional lights buffer
        vk::DescriptorBindingFlags { }, // shadow caster directional lights buffer
        vk::DescriptorBindingFlags { }, // bones buffer
        vk::DescriptorBindingFlags { }, // reflection probes buffer
        vk::DescriptorBindingFlags { }, // cameras joint models buffer
    };

    vk::DescriptorSetLayoutBindingFlagsCreateInfo binding_flags_info;
    binding_flags_info.setBindingFlags(binding_flags);

    vk::DescriptorSetLayoutCreateInfo layout_info;
    layout_info.setBindings(bindings);
    layout_info.flags = vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool;
    layout_info.pNext = &binding_flags_info;

    descriptor_set_layout = vk::raii::DescriptorSetLayout(dev_raii, layout_info);

    // ========== Pipeline Layout ==========
    constexpr vk::PushConstantRange push_constant_range { vk::ShaderStageFlagBits::eAll, 0, sizeof(pipeline::PushConstants) };
    const auto descriptor_set_layout_raw = *descriptor_set_layout;
    const vk::PipelineLayoutCreateInfo pipeline_layout_info { { }, descriptor_set_layout_raw, push_constant_range };

    pipeline_layout = vk::raii::PipelineLayout(dev_raii, pipeline_layout_info);

    // ========== Descriptor Pool ==========
    constexpr std::array pool_sizes {
        vk::DescriptorPoolSize { vk::DescriptorType::eSampledImage, max_images },
        vk::DescriptorPoolSize { vk::DescriptorType::eSampler, max_samplers + max_shadow_samplers },
        vk::DescriptorPoolSize { vk::DescriptorType::eStorageBuffer, 10 },
    };
    const vk::DescriptorPoolCreateInfo pool_info {
        vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind | vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
        1, pool_sizes
    };

    descriptor_pool = vk::raii::DescriptorPool(dev_raii, pool_info);

    // ========== Allocate Descriptor Set ==========
    vk::DescriptorSetAllocateInfo allocate_info;
    allocate_info.descriptorPool = *descriptor_pool;
    allocate_info.setSetLayouts(descriptor_set_layout_raw);

    auto sets = dev.allocateDescriptorSets(allocate_info);
    descriptor_set = sets[0];

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

    free_shadow_2d_image_indices.reserve(max_shadow_2d_images);
    for (std::uint32_t i = max_shadow_2d_images; i > 0;) {
        --i;
        free_shadow_2d_image_indices.push_back(i);
    }

    free_sampler_indices.reserve(max_samplers);
    for (std::uint32_t i = max_samplers; i > 0;) {
        --i;
        free_sampler_indices.push_back(i);
    }

    // ========== Initialize Buffer Descriptors ==========
    const std::array buffers {
        &scenes_buffer,
        &cameras_buffer,
        &models_buffer,
        &materials_buffer,
        &point_lights_buffer,
        &directional_lights_buffer,
        &shadow_caster_directional_lights_buffer,
        &bones_buffer,
        &reflection_probes_buffer,
        &cameras_joint_models_buffer
    };

    std::array<vk::DescriptorBufferInfo, 10> buffer_infos { };
    std::array<vk::WriteDescriptorSet, 10> writes { };

    for (std::uint32_t i = 0; i < buffers.size(); ++i) {
        const auto& buff = *buffers[i];
        const auto& allocator = *buff.get_allocated_memory()->get_allocator();
        buffer_infos[i].buffer = buff.get_vulkan_data();
        buffer_infos[i].offset = static_cast<vk::DeviceSize>(buff.get_offset());
        buffer_infos[i].range = static_cast<vk::DeviceSize>(allocator.get_size());

        writes[i].dstSet = descriptor_set;
        writes[i].dstBinding = 7 + i; // Buffers start at binding 7
        writes[i].descriptorType = vk::DescriptorType::eStorageBuffer;
        writes[i].setBufferInfo(buffer_infos[i]);
    }

    dev.updateDescriptorSets(writes, { });

    // ========== Create Shadow Comparison Sampler ==========
    vk::SamplerCreateInfo shadow_sampler_info;
    shadow_sampler_info.magFilter = vk::Filter::eLinear;
    shadow_sampler_info.minFilter = vk::Filter::eLinear;
    shadow_sampler_info.addressModeU = vk::SamplerAddressMode::eClampToEdge;
    shadow_sampler_info.addressModeV = vk::SamplerAddressMode::eClampToEdge;
    shadow_sampler_info.addressModeW = vk::SamplerAddressMode::eClampToEdge;
    shadow_sampler_info.anisotropyEnable = false;
    shadow_sampler_info.compareEnable = true;
    shadow_sampler_info.compareOp = vk::CompareOp::eLessOrEqual;
    shadow_sampler_info.mipmapMode = vk::SamplerMipmapMode::eNearest;
    shadow_sampler_info.borderColor = vk::BorderColor::eFloatOpaqueWhite;

    shadow_sampler = vk::raii::Sampler(dev_raii, shadow_sampler_info);

    const vk::DescriptorImageInfo shadow_sampler_descriptor_info { *shadow_sampler };

    vk::WriteDescriptorSet shadow_sampler_write;
    shadow_sampler_write.dstSet = descriptor_set;
    shadow_sampler_write.dstBinding = 6;
    shadow_sampler_write.descriptorType = vk::DescriptorType::eSampler;
    shadow_sampler_write.setImageInfo(shadow_sampler_descriptor_info);

    dev.updateDescriptorSets(shadow_sampler_write, { });
}

void gearoenix::vulkan::descriptor::Bindless::write_image_descriptor(const std::uint32_t binding, const std::uint32_t index, const vk::ImageView view, const vk::ImageLayout layout) const
{
    vk::DescriptorImageInfo info;
    info.imageView = view;
    info.imageLayout = layout;

    vk::WriteDescriptorSet write;
    write.dstSet = descriptor_set;
    write.dstBinding = binding;
    write.dstArrayElement = index;
    write.descriptorType = vk::DescriptorType::eSampledImage;

    core::job::send_job(render::engine::Engine::get().get_jobs_thread_id(), [write, info]() mutable {
        write.setImageInfo(info);
        device::Logical::get().get_vulkan_data().updateDescriptorSets(write, { });
    });
}

void gearoenix::vulkan::descriptor::Bindless::write_sampler_descriptor(const std::uint32_t index, const vk::Sampler sampler) const
{
    vk::DescriptorImageInfo info;
    info.sampler = sampler;

    vk::WriteDescriptorSet write;
    write.dstSet = descriptor_set;
    write.dstBinding = 5; // Sampler binding
    write.dstArrayElement = index;
    write.descriptorType = vk::DescriptorType::eSampler;

    core::job::send_job(render::engine::Engine::get().get_jobs_thread_id(), [write, info]() mutable {
        write.setImageInfo(info);
        device::Logical::get().get_vulkan_data().updateDescriptorSets(write, { });
    });
}

std::uint32_t gearoenix::vulkan::descriptor::Bindless::allocate_1d_image(const vk::ImageView view, const vk::ImageLayout layout)
{
    GX_ASSERT_D(view);
    const std::lock_guard _lg(allocation_lock);
    GX_ASSERT_D(!free_1d_image_indices.empty());
    const auto index = free_1d_image_indices.back();
    free_1d_image_indices.pop_back();

    write_image_descriptor(0, index, view, layout);
    return index;
}

std::uint32_t gearoenix::vulkan::descriptor::Bindless::allocate_2d_image(const vk::ImageView view, const vk::ImageLayout layout)
{
    GX_ASSERT_D(view);
    const std::lock_guard _lg(allocation_lock);
    GX_ASSERT_D(!free_2d_image_indices.empty());
    const auto index = free_2d_image_indices.back();
    free_2d_image_indices.pop_back();

    write_image_descriptor(1, index, view, layout);
    return index;
}

std::uint32_t gearoenix::vulkan::descriptor::Bindless::allocate_3d_image(const vk::ImageView view, const vk::ImageLayout layout)
{
    GX_ASSERT_D(view);
    const std::lock_guard _lg(allocation_lock);
    GX_ASSERT_D(!free_3d_image_indices.empty());
    const auto index = free_3d_image_indices.back();
    free_3d_image_indices.pop_back();

    write_image_descriptor(2, index, view, layout);
    return index;
}

std::uint32_t gearoenix::vulkan::descriptor::Bindless::allocate_cube_image(const vk::ImageView view, const vk::ImageLayout layout)
{
    GX_ASSERT_D(view);
    const std::lock_guard _lg(allocation_lock);
    GX_ASSERT_D(!free_cube_image_indices.empty());
    const auto index = free_cube_image_indices.back();
    free_cube_image_indices.pop_back();

    write_image_descriptor(3, index, view, layout);
    return index;
}

std::uint32_t gearoenix::vulkan::descriptor::Bindless::allocate_shadow_2d_image(const vk::ImageView view, const vk::ImageLayout layout)
{
    GX_ASSERT_D(view);
    const std::lock_guard _lg(allocation_lock);
    GX_ASSERT_D(!free_shadow_2d_image_indices.empty());
    const auto index = free_shadow_2d_image_indices.back();
    free_shadow_2d_image_indices.pop_back();

    write_image_descriptor(4, index, view, layout);
    return index;
}

std::uint32_t gearoenix::vulkan::descriptor::Bindless::allocate_sampler(const vk::Sampler sampler)
{
    GX_ASSERT_D(sampler);
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
    // No need to write null descriptor - PARTIALLY_BOUND_BIT allows unaccessed descriptors to be invalid
    free_1d_image_indices.push_back(index);
}

void gearoenix::vulkan::descriptor::Bindless::free_2d_image(const std::uint32_t index)
{
    const std::lock_guard _lg(allocation_lock);
    GX_ASSERT_D(index < max_2d_images);
    // No need to write null descriptor - PARTIALLY_BOUND_BIT allows unaccessed descriptors to be invalid
    free_2d_image_indices.push_back(index);
}

void gearoenix::vulkan::descriptor::Bindless::free_3d_image(const std::uint32_t index)
{
    const std::lock_guard _lg(allocation_lock);
    GX_ASSERT_D(index < max_3d_images);
    // No need to write null descriptor - PARTIALLY_BOUND_BIT allows unaccessed descriptors to be invalid
    free_3d_image_indices.push_back(index);
}

void gearoenix::vulkan::descriptor::Bindless::free_cube_image(const std::uint32_t index)
{
    const std::lock_guard _lg(allocation_lock);
    GX_ASSERT_D(index < max_cube_images);
    // No need to write null descriptor - PARTIALLY_BOUND_BIT allows unaccessed descriptors to be invalid
    free_cube_image_indices.push_back(index);
}

void gearoenix::vulkan::descriptor::Bindless::free_shadow_2d_image(const std::uint32_t index)
{
    const std::lock_guard _lg(allocation_lock);
    GX_ASSERT_D(index < max_shadow_2d_images);
    // No need to write null descriptor - PARTIALLY_BOUND_BIT allows unaccessed descriptors to be invalid
    free_shadow_2d_image_indices.push_back(index);
}

void gearoenix::vulkan::descriptor::Bindless::free_sampler(const std::uint32_t index)
{
    const std::lock_guard _lg(allocation_lock);
    GX_ASSERT_D(index < max_samplers);
    // No need to write null descriptor - PARTIALLY_BOUND_BIT allows unaccessed descriptors to be invalid
    free_sampler_indices.push_back(index);
}

void gearoenix::vulkan::descriptor::Bindless::bind(const vk::CommandBuffer cmd) const
{
    cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipeline_layout, 0, descriptor_set, { });
}

#endif
