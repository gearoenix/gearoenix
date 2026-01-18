#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../gx-vk-loader.hpp"
#include "../gx-vk-build-configuration.hpp"
#include "../../core/gx-cr-singleton.hpp"

#include <array>
#include <mutex>
#include <optional>
#include <vector>

namespace gearoenix::vulkan::buffer {
struct Buffer;
}

namespace gearoenix::vulkan::descriptor {
struct Bindless final: core::Singleton<Bindless> {
    static constexpr std::uint32_t max_1d_images = 8;
    static constexpr std::uint32_t max_2d_images = 8192;
    static constexpr std::uint32_t max_3d_images = 8;
    static constexpr std::uint32_t max_cube_images = 32;
    static constexpr std::uint32_t max_images = max_1d_images + max_2d_images + max_3d_images + max_cube_images;
    static constexpr std::uint32_t max_samplers = 32;

private:
    VkDescriptorSetLayout descriptor_set_layout = VK_NULL_HANDLE;
    VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;
    VkDescriptorSet descriptor_set = VK_NULL_HANDLE;
    
    std::vector<std::uint32_t> free_1d_image_indices = { };
    std::vector<std::uint32_t> free_2d_image_indices = { };
    std::vector<std::uint32_t> free_3d_image_indices = { };
    std::vector<std::uint32_t> free_cube_image_indices = { };
    std::vector<std::uint32_t> free_sampler_indices = { };

    std::vector<VkDescriptorImageInfo> image_1d_infos;
    std::vector<VkDescriptorImageInfo> image_2d_infos;
    std::vector<VkDescriptorImageInfo> image_3d_infos;
    std::vector<VkDescriptorImageInfo> image_cube_infos;
    std::vector<VkDescriptorImageInfo> sampler_infos;
    
    std::mutex allocation_lock;

    void write_image_descriptor(std::uint32_t binding, std::uint32_t index, const VkDescriptorImageInfo& info) const;
    void write_sampler_descriptor(std::uint32_t index, const VkDescriptorImageInfo& info) const;

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

    [[nodiscard]] VkDescriptorSetLayout get_descriptor_set_layout() const;
    [[nodiscard]] VkDescriptorSet get_descriptor_set() const;

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
};
}
#endif
