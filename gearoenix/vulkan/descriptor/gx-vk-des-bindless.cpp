#include "gx-vk-des-bindless.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/allocator/gx-cr-alc-range.hpp"
#include "../../core/job/gx-cr-job-manager.hpp"
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../../render/buffer/gx-rnd-buf-manager.hpp"
#include "../buffer/gx-vk-buf-buffer.hpp"
#include "../buffer/gx-vk-buf-manager.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-marker.hpp"
#include "../image/gx-vk-img-view.hpp"
#include "../pipeline/gx-vk-pip-push-constant.hpp"

namespace {
constexpr vk::DescriptorBindingFlags gx_binding_flags = vk::DescriptorBindingFlagBits::ePartiallyBound | vk::DescriptorBindingFlagBits::eUpdateAfterBind | vk::DescriptorBindingFlagBits::eUpdateUnusedWhilePending;
}

gearoenix::vulkan::descriptor::Bindless::Bindless()
    : Singleton(this)
    , descriptor_set_count(device::Physical::get().get_unified_memory() ? frames_in_flight : 1)
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
    GX_VK_MARK("gx-vk-main-descriptor-set-layout", *descriptor_set_layout);

    // ========== Pipeline Layout ==========
    constexpr vk::PushConstantRange push_constant_range { vk::ShaderStageFlagBits::eAll, 0, sizeof(pipeline::PushConstants) };
    const auto descriptor_set_layout_raw = *descriptor_set_layout;
    const vk::PipelineLayoutCreateInfo pipeline_layout_info { { }, descriptor_set_layout_raw, push_constant_range };

    pipeline_layout = vk::raii::PipelineLayout(dev_raii, pipeline_layout_info);
    GX_VK_MARK("gx-vk-main-pipeline-layout", *pipeline_layout);

    // ========== Descriptor Pool ==========
    const std::array pool_sizes {
        vk::DescriptorPoolSize { vk::DescriptorType::eSampledImage, max_images * descriptor_set_count },
        vk::DescriptorPoolSize { vk::DescriptorType::eSampler, (max_samplers + max_shadow_samplers) * descriptor_set_count },
        vk::DescriptorPoolSize { vk::DescriptorType::eStorageBuffer, buffer_bindings_count * descriptor_set_count },
    };
    const vk::DescriptorPoolCreateInfo pool_info {
        vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind | vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
        descriptor_set_count, pool_sizes
    };

    descriptor_pool = vk::raii::DescriptorPool(dev_raii, pool_info);
    GX_VK_MARK("gx-vk-main-descriptor-pool", *descriptor_pool);

    // ========== Allocate Descriptor Sets ==========
    const std::vector layouts(descriptor_set_count, descriptor_set_layout_raw);
    vk::DescriptorSetAllocateInfo allocate_info;
    allocate_info.descriptorPool = *descriptor_pool;
    allocate_info.setSetLayouts(layouts);

    descriptor_sets = dev.allocateDescriptorSets(allocate_info);
    for (std::uint32_t s = 0; s < descriptor_set_count; ++s) {
        GX_VK_MARK("gx-vk-main-descriptor-set-" + std::to_string(s), descriptor_sets[s]);
    }

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
    using RegionIndex = render::buffer::UniformRegionIndex;
    constexpr std::array<RegionIndex, buffer_bindings_count> region_order {
        RegionIndex::scenes,
        RegionIndex::cameras,
        RegionIndex::models,
        RegionIndex::materials,
        RegionIndex::point_lights,
        RegionIndex::directional_lights,
        RegionIndex::shadow_caster_directional_lights,
        RegionIndex::bones,
        RegionIndex::reflection_probes,
        RegionIndex::camera_joint_model,
    };

    const auto is_unified = device::Physical::get().get_unified_memory();
    const auto& buffer_manager = core::Singleton<buffer::Manager>::get();

    for (std::uint32_t s = 0; s < descriptor_set_count; ++s) {
        const auto& base_buffer = is_unified
            ? *buffer_manager.get_each_frame(s)
            : *buffer_manager.get_each_frame_destination();
        const auto base_offset = static_cast<vk::DeviceSize>(base_buffer.get_offset());
        const auto base_vulkan_buffer = base_buffer.get_vulkan_data();

        std::array<vk::DescriptorBufferInfo, buffer_bindings_count> buffer_infos { };
        std::array<vk::WriteDescriptorSet, buffer_bindings_count> writes { };

        for (std::uint32_t i = 0; i < buffer_bindings_count; ++i) {
            const auto& region = *render::buffer::Manager::get_region(region_order[i]);
            buffer_infos[i].buffer = base_vulkan_buffer;
            buffer_infos[i].offset = base_offset + static_cast<vk::DeviceSize>(region.get_offset());
            buffer_infos[i].range = static_cast<vk::DeviceSize>(region.get_size());

            writes[i].dstSet = descriptor_sets[s];
            writes[i].dstBinding = 7 + i; // Buffers start at binding 7
            writes[i].descriptorType = vk::DescriptorType::eStorageBuffer;
            writes[i].setBufferInfo(buffer_infos[i]);
        }

        dev.updateDescriptorSets(writes, { });
    }

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

    // Write shadow sampler to all descriptor sets
    for (std::uint32_t s = 0; s < descriptor_set_count; ++s) {
        const vk::DescriptorImageInfo shadow_sampler_descriptor_info { *shadow_sampler };

        vk::WriteDescriptorSet shadow_sampler_write;
        shadow_sampler_write.dstSet = descriptor_sets[s];
        shadow_sampler_write.dstBinding = 6;
        shadow_sampler_write.descriptorType = vk::DescriptorType::eSampler;
        shadow_sampler_write.setImageInfo(shadow_sampler_descriptor_info);

        dev.updateDescriptorSets(shadow_sampler_write, { });
    }
}

void gearoenix::vulkan::descriptor::Bindless::write_image_descriptor(
    const std::uint32_t binding, const std::uint32_t index, const std::shared_ptr<image::View>& view, const vk::ImageLayout layout) const
{
    core::job::send_job(engine::Engine::get_jobs_thread_id(), [this, binding, index, layout, view]() {
        vk::DescriptorImageInfo info;
        info.imageView = view->get_vulkan_data();
        info.imageLayout = layout;

        for (std::uint32_t s = 0; s < descriptor_set_count; ++s) {
            vk::WriteDescriptorSet write;
            write.dstSet = descriptor_sets[s];
            write.dstBinding = binding;
            write.dstArrayElement = index;
            write.descriptorType = vk::DescriptorType::eSampledImage;
            write.setImageInfo(info);
            device::Logical::get().get_vulkan_data().updateDescriptorSets(write, { });
        }
    });
}

void gearoenix::vulkan::descriptor::Bindless::write_sampler_descriptor(const std::uint32_t index, const vk::Sampler sampler) const
{
    core::job::send_job(engine::Engine::get_jobs_thread_id(), [this, index, sampler]() {
        vk::DescriptorImageInfo info;
        info.sampler = sampler;

        for (std::uint32_t s = 0; s < descriptor_set_count; ++s) {
            vk::WriteDescriptorSet write;
            write.dstSet = descriptor_sets[s];
            write.dstBinding = 5;
            write.dstArrayElement = index;
            write.descriptorType = vk::DescriptorType::eSampler;
            write.setImageInfo(info);
            device::Logical::get().get_vulkan_data().updateDescriptorSets(write, { });
        }
    });
}

std::uint32_t gearoenix::vulkan::descriptor::Bindless::allocate_1d_image(const std::shared_ptr<image::View>& view, const vk::ImageLayout layout)
{
    GX_ASSERT_D(view);
    const std::lock_guard _lg(allocation_lock);
    GX_ASSERT_D(!free_1d_image_indices.empty());
    const auto index = free_1d_image_indices.back();
    free_1d_image_indices.pop_back();

    write_image_descriptor(0, index, view, layout);
    return index;
}

std::uint32_t gearoenix::vulkan::descriptor::Bindless::allocate_2d_image(const std::shared_ptr<image::View>& view, const vk::ImageLayout layout)
{
    GX_ASSERT_D(view);
    const std::lock_guard _lg(allocation_lock);
    GX_ASSERT_D(!free_2d_image_indices.empty());
    const auto index = free_2d_image_indices.back();
    free_2d_image_indices.pop_back();

    write_image_descriptor(1, index, view, layout);
    return index;
}

std::uint32_t gearoenix::vulkan::descriptor::Bindless::allocate_3d_image(const std::shared_ptr<image::View>& view, const vk::ImageLayout layout)
{
    GX_ASSERT_D(view);
    const std::lock_guard _lg(allocation_lock);
    GX_ASSERT_D(!free_3d_image_indices.empty());
    const auto index = free_3d_image_indices.back();
    free_3d_image_indices.pop_back();

    write_image_descriptor(2, index, view, layout);
    return index;
}

std::uint32_t gearoenix::vulkan::descriptor::Bindless::allocate_cube_image(const std::shared_ptr<image::View>& view, const vk::ImageLayout layout)
{
    GX_ASSERT_D(view);
    const std::lock_guard _lg(allocation_lock);
    GX_ASSERT_D(!free_cube_image_indices.empty());
    const auto index = free_cube_image_indices.back();
    free_cube_image_indices.pop_back();

    write_image_descriptor(3, index, view, layout);
    return index;
}

std::uint32_t gearoenix::vulkan::descriptor::Bindless::allocate_shadow_2d_image(const std::shared_ptr<image::View>& view, const vk::ImageLayout layout)
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
    const auto frame_number = engine::Engine::get_frame_number();
    const auto set_index = frame_number % descriptor_set_count;
    cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipeline_layout, 0, descriptor_sets[set_index], { });
}

vk::DescriptorSet gearoenix::vulkan::descriptor::Bindless::get_current_descriptor_set() const
{
    const auto frame_number = engine::Engine::get_frame_number();
    const auto set_index = frame_number % descriptor_set_count;
    return descriptor_sets[set_index];
}

#endif