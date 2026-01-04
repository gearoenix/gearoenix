#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../gx-vk-loader.hpp"
#include <mutex>
#include <optional>
#include <vector>

namespace gearoenix::vulkan::device {
struct Logical;
}

namespace gearoenix::vulkan::descriptor {
struct Bindless final {
    static constexpr std::uint32_t max_sampled_images = 8192;

private:
    const device::Logical& logical_device;
    VkDescriptorSetLayout set_layout = VK_NULL_HANDLE;
    VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;
    VkDescriptorSet descriptor_set = VK_NULL_HANDLE;
    std::vector<std::uint32_t> free_sampled_indices;
    std::vector<VkDescriptorImageInfo> sampled_image_infos;
    std::mutex sampled_allocation_lock;

    void write_descriptor(std::uint32_t index, const VkDescriptorImageInfo& info);

public:
    explicit Bindless(const device::Logical& logical_device);
    ~Bindless();

    Bindless(const Bindless&) = delete;
    Bindless(Bindless&&) = delete;
    Bindless& operator=(const Bindless&) = delete;
    Bindless& operator=(Bindless&&) = delete;

    [[nodiscard]] VkDescriptorSetLayout get_set_layout() const;
    [[nodiscard]] VkDescriptorSet get_descriptor_set() const;

    [[nodiscard]] std::optional<std::uint32_t> allocate_sampled_image(
        VkImageView view,
        VkSampler sampler,
        VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    void free_sampled_image(std::uint32_t index);
};
}
#endif
