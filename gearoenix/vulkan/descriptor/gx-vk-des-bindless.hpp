#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../gx-vk-loader.hpp"

#include <boost/container/static_vector.hpp>

#include <mutex>
#include <optional>
#include <vector>

namespace gearoenix::vulkan::device {
struct Logical;
}

namespace gearoenix::vulkan::descriptor {
struct Bindless final {
    static constexpr std::uint32_t max_1d_images = 8;
    static constexpr std::uint32_t max_2d_images = 8192;
    static constexpr std::uint32_t max_3d_images = 32;
    static constexpr std::uint32_t max_cube_images = 512;
    static constexpr std::uint32_t max_images = max_1d_images + max_2d_images + max_3d_images + max_cube_images;
    static constexpr std::uint32_t max_samplers = 32;

private:
    VkDescriptorSetLayout set_layout = VK_NULL_HANDLE;
    VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;
    VkDescriptorSet static_descriptor_set = VK_NULL_HANDLE;
    boost::container::static_vector<VkDescriptorSet, 4> dynamic_descriptor_sets;
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

    void write_descriptor(std::uint32_t index, const VkDescriptorImageInfo& info);

public:
    Bindless();
    ~Bindless();

    Bindless(Bindless&&) = delete;
    Bindless(const Bindless&) = delete;
    Bindless& operator=(Bindless&&) = delete;
    Bindless& operator=(const Bindless&) = delete;

    [[nodiscard]] VkDescriptorSetLayout get_set_layout() const;
    [[nodiscard]] VkDescriptorSet get_static_descriptor_set() const;
    [[nodiscard]] VkDescriptorSet get_per_frame_descriptor_set(std::uint32_t frame_index) const;

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
};
}
#endif
