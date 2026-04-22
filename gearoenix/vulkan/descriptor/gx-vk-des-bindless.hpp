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
    static constexpr std::uint32_t buffer_bindings_count = 10;

private:
    const std::uint32_t descriptor_set_count;
    vk::raii::DescriptorSetLayout descriptor_set_layout { nullptr };
    vk::raii::DescriptorPool descriptor_pool { nullptr };
    std::vector<vk::DescriptorSet> descriptor_sets;
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
    Bindless();
    ~Bindless() override = default;

    Bindless(Bindless&&) = delete;
    Bindless(const Bindless&) = delete;
    Bindless& operator=(Bindless&&) = delete;
    Bindless& operator=(const Bindless&) = delete;

    [[nodiscard]] vk::DescriptorSetLayout get_descriptor_set_layout() const { return *descriptor_set_layout; }
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
