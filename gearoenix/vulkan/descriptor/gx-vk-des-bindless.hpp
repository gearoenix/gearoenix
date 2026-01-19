#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/gx-cr-singleton.hpp"
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-build-configuration.hpp"
#include "../gx-vk-loader.hpp"

#include <mutex>
#include <vector>

namespace gearoenix::vulkan::buffer {
struct Buffer;
}

namespace gearoenix::vulkan::descriptor {
struct BindlessPushConstants final {
    std::uint32_t scene_index = 0;
    std::uint32_t camera_index = 0;
    std::uint32_t model_index = 0;
    std::uint32_t material_index = 0;
    std::uint32_t light_index = 0;
};

struct Bindless final : core::Singleton<Bindless> {
    static constexpr std::uint32_t max_1d_images = 8;
    static constexpr std::uint32_t max_2d_images = 8192;
    static constexpr std::uint32_t max_3d_images = 8;
    static constexpr std::uint32_t max_cube_images = 32;
    static constexpr std::uint32_t max_images = max_1d_images + max_2d_images + max_3d_images + max_cube_images;
    static constexpr std::uint32_t max_samplers = 32;

    GX_GET_VAL_PRV(VkDescriptorSetLayout, descriptor_set_layout, VK_NULL_HANDLE);
    GX_GET_VAL_PRV(VkDescriptorPool, descriptor_pool, VK_NULL_HANDLE);
    GX_GET_VAL_PRV(VkDescriptorSet, descriptor_set, VK_NULL_HANDLE);
    GX_GET_VAL_PRV(VkPipelineLayout, pipeline_layout, VK_NULL_HANDLE);

    std::vector<std::uint32_t> free_1d_image_indices = { };
    std::vector<std::uint32_t> free_2d_image_indices = { };
    std::vector<std::uint32_t> free_3d_image_indices = { };
    std::vector<std::uint32_t> free_cube_image_indices = { };
    std::vector<std::uint32_t> free_sampler_indices = { };

    std::mutex allocation_lock;

    void write_image_descriptor(std::uint32_t binding, std::uint32_t index, VkImageView view, VkImageLayout layout) const;
    void write_sampler_descriptor(std::uint32_t index, VkSampler sampler) const;

public:
    Bindless(
        const buffer::Buffer& scenes_buffer,
        const buffer::Buffer& cameras_buffer,
        const buffer::Buffer& models_buffer,
        const buffer::Buffer& materials_buffer,
        const buffer::Buffer& lights_buffer);
    ~Bindless() override;

    Bindless(Bindless&&) = delete;
    Bindless(const Bindless&) = delete;
    Bindless& operator=(Bindless&&) = delete;
    Bindless& operator=(const Bindless&) = delete;

    [[nodiscard]] std::uint32_t allocate_1d_image(VkImageView view, VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    [[nodiscard]] std::uint32_t allocate_2d_image(VkImageView view, VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    [[nodiscard]] std::uint32_t allocate_3d_image(VkImageView view, VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    [[nodiscard]] std::uint32_t allocate_cube_image(VkImageView view, VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    [[nodiscard]] std::uint32_t allocate_sampler(VkSampler sampler);

    void free_1d_image(std::uint32_t index);
    void free_2d_image(std::uint32_t index);
    void free_3d_image(std::uint32_t index);
    void free_cube_image(std::uint32_t index);
    void free_sampler(std::uint32_t index);

    void bind(VkCommandBuffer) const;
};
}
#endif