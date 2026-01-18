#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../gx-vk-loader.hpp"

#include <array>
#include <mutex>
#include <optional>
#include <vector>

namespace gearoenix::vulkan::device {
struct Logical;
}

namespace gearoenix::vulkan::descriptor {
struct Bindless final {
    static constexpr std::uint32_t frames_in_flight = 3;
    static constexpr std::uint32_t max_1d_images = 8;
    static constexpr std::uint32_t max_2d_images = 8192;
    static constexpr std::uint32_t max_3d_images = 32;
    static constexpr std::uint32_t max_cube_images = 512;
    static constexpr std::uint32_t max_images = max_1d_images + max_2d_images + max_3d_images + max_cube_images;
    static constexpr std::uint32_t max_samplers = 32;
    static constexpr std::uint32_t max_scenes = 64;
    static constexpr std::uint32_t max_cameras = 128;
    static constexpr std::uint32_t max_models = 16384;
    static constexpr std::uint32_t max_materials = 4096;
    static constexpr std::uint32_t max_lights = 1024;

private:
    VkDescriptorSetLayout static_set_layout = VK_NULL_HANDLE;
    VkDescriptorSetLayout dynamic_set_layout = VK_NULL_HANDLE;
    VkDescriptorPool static_descriptor_pool = VK_NULL_HANDLE;
    VkDescriptorPool dynamic_descriptor_pool = VK_NULL_HANDLE;
    VkDescriptorSet static_descriptor_set = VK_NULL_HANDLE;
    std::array<VkDescriptorSet, frames_in_flight> dynamic_descriptor_sets;
    std::vector<std::uint32_t> free_1d_image_indices;
    std::vector<std::uint32_t> free_2d_image_indices;
    std::vector<std::uint32_t> free_3d_image_indices;
    std::vector<std::uint32_t> free_cube_image_indices;
    std::vector<std::uint32_t> free_sampler_indices;
    std::vector<VkDescriptorImageInfo> image_1d_infos;
    std::vector<VkDescriptorImageInfo> image_2d_infos;
    std::vector<VkDescriptorImageInfo> image_3d_infos;
    std::vector<VkDescriptorImageInfo> image_cube_infos;
    std::vector<VkDescriptorImageInfo> sampler_infos;
    std::mutex allocation_lock;

    void write_image_descriptor(std::uint32_t binding, std::uint32_t index, const VkDescriptorImageInfo& info);
    void write_sampler_descriptor(std::uint32_t index, const VkDescriptorImageInfo& info);

public:
    Bindless();
    ~Bindless();

    Bindless(Bindless&&) = delete;
    Bindless(const Bindless&) = delete;
    Bindless& operator=(Bindless&&) = delete;
    Bindless& operator=(const Bindless&) = delete;

    [[nodiscard]] VkDescriptorSetLayout get_static_set_layout() const;
    [[nodiscard]] VkDescriptorSetLayout get_dynamic_set_layout() const;
    [[nodiscard]] VkDescriptorSet get_static_descriptor_set() const;
    [[nodiscard]] VkDescriptorSet get_dynamic_descriptor_set(std::uint32_t frame_index) const;

    [[nodiscard]] std::optional<std::uint32_t> allocate_1d_image(VkImageView view, VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    [[nodiscard]] std::optional<std::uint32_t> allocate_2d_image(VkImageView view, VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    [[nodiscard]] std::optional<std::uint32_t> allocate_3d_image(VkImageView view, VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    [[nodiscard]] std::optional<std::uint32_t> allocate_cube_image(VkImageView view, VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    [[nodiscard]] std::optional<std::uint32_t> allocate_sampler(VkSampler sampler);

    void free_1d_image(std::uint32_t index);
    void free_2d_image(std::uint32_t index);
    void free_3d_image(std::uint32_t index);
    void free_cube_image(std::uint32_t index);
    void free_sampler(std::uint32_t index);

    void update_dynamic_buffers(std::uint32_t frame_index, VkBuffer scenes_buffer, VkBuffer cameras_buffer, VkBuffer models_buffer, VkBuffer materials_buffer, VkBuffer lights_buffer);
};
}
#endif
