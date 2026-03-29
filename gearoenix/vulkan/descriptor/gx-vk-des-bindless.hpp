#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/gx-cr-singleton.hpp"
#include "../gx-vk-build-configuration.hpp"
#include "../gx-vk-loader.hpp"

#include <mutex>
#include <vector>

namespace gearoenix::vulkan::buffer {
struct Buffer;
}

namespace gearoenix::vulkan::image {
struct View;
}

namespace gearoenix::vulkan::descriptor {
struct Bindless final : core::Singleton<Bindless> {
    static constexpr std::uint32_t max_1d_images = 8;
    static constexpr std::uint32_t max_2d_images = 8192;
    static constexpr std::uint32_t max_3d_images = 8;
    static constexpr std::uint32_t max_cube_images = 32;
    static constexpr std::uint32_t max_shadow_2d_images = 256;
    static constexpr std::uint32_t max_images = max_1d_images + max_2d_images + max_3d_images + max_cube_images + max_shadow_2d_images;
    static constexpr std::uint32_t max_samplers = 32;
    static constexpr std::uint32_t max_shadow_samplers = 1;

private:
    vk::raii::DescriptorSetLayout descriptor_set_layout { nullptr };
    vk::raii::DescriptorPool descriptor_pool { nullptr };
    vk::DescriptorSet descriptor_set;
    vk::raii::PipelineLayout pipeline_layout { nullptr };
    vk::raii::Sampler shadow_sampler { nullptr };

    std::vector<std::uint32_t> free_1d_image_indices = { };
    std::vector<std::uint32_t> free_2d_image_indices = { };
    std::vector<std::uint32_t> free_3d_image_indices = { };
    std::vector<std::uint32_t> free_cube_image_indices = { };
    std::vector<std::uint32_t> free_shadow_2d_image_indices = { };
    std::vector<std::uint32_t> free_sampler_indices = { };

    std::mutex allocation_lock;

    void write_image_descriptor(std::uint32_t binding, std::uint32_t index, const std::shared_ptr<image::View>& view, vk::ImageLayout layout) const;
    void write_sampler_descriptor(std::uint32_t index, vk::Sampler sampler) const;

public:
    Bindless(
        const buffer::Buffer& scenes_buffer,
        const buffer::Buffer& cameras_buffer,
        const buffer::Buffer& models_buffer,
        const buffer::Buffer& materials_buffer,
        const buffer::Buffer& point_lights_buffer,
        const buffer::Buffer& directional_lights_buffer,
        const buffer::Buffer& shadow_caster_directional_lights_buffer,
        const buffer::Buffer& bones_buffer,
        const buffer::Buffer& reflection_probes_buffer,
        const buffer::Buffer& cameras_joint_models_buffer);
    ~Bindless() override = default;

    Bindless(Bindless&&) = delete;
    Bindless(const Bindless&) = delete;
    Bindless& operator=(Bindless&&) = delete;
    Bindless& operator=(const Bindless&) = delete;

    [[nodiscard]] vk::DescriptorSetLayout get_descriptor_set_layout() const { return *descriptor_set_layout; }
    [[nodiscard]] vk::DescriptorPool get_descriptor_pool() const { return *descriptor_pool; }
    [[nodiscard]] vk::DescriptorSet get_descriptor_set() const { return descriptor_set; }
    [[nodiscard]] vk::PipelineLayout get_pipeline_layout() const { return *pipeline_layout; }
    [[nodiscard]] vk::Sampler get_shadow_sampler() const { return *shadow_sampler; }

    [[nodiscard]] std::uint32_t allocate_1d_image(const std::shared_ptr<image::View>& view, vk::ImageLayout layout = vk::ImageLayout::eShaderReadOnlyOptimal);
    [[nodiscard]] std::uint32_t allocate_2d_image(const std::shared_ptr<image::View>& view, vk::ImageLayout layout = vk::ImageLayout::eShaderReadOnlyOptimal);
    [[nodiscard]] std::uint32_t allocate_3d_image(const std::shared_ptr<image::View>& view, vk::ImageLayout layout = vk::ImageLayout::eShaderReadOnlyOptimal);
    [[nodiscard]] std::uint32_t allocate_cube_image(const std::shared_ptr<image::View>& view, vk::ImageLayout layout = vk::ImageLayout::eShaderReadOnlyOptimal);
    [[nodiscard]] std::uint32_t allocate_shadow_2d_image(const std::shared_ptr<image::View>& view, vk::ImageLayout layout = vk::ImageLayout::eShaderReadOnlyOptimal);
    [[nodiscard]] std::uint32_t allocate_sampler(vk::Sampler sampler);

    void free_1d_image(std::uint32_t index);
    void free_2d_image(std::uint32_t index);
    void free_3d_image(std::uint32_t index);
    void free_cube_image(std::uint32_t index);
    void free_shadow_2d_image(std::uint32_t index);
    void free_sampler(std::uint32_t index);

    void bind(vk::CommandBuffer cmd) const;
};
}
#endif
