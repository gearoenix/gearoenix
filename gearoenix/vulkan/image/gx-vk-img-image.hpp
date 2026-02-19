#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"

#include <memory>
#include <string>
#include <vector>

namespace gearoenix::vulkan::memory {
struct Memory;
}

namespace gearoenix::vulkan::image {
/// Describes the desired state for an image transition.
/// Users only specify what they NEED - the image tracks the current state internally.
struct TransitionRequest final {
    VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
    VkAccessFlags2 access = 0;
    VkPipelineStageFlags2 stage = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
    std::uint32_t queue_family = VK_QUEUE_FAMILY_IGNORED;
    std::uint32_t base_mip = 0;
    std::uint32_t mip_count = VK_REMAINING_MIP_LEVELS;
    std::uint32_t base_layer = 0;
    std::uint32_t layer_count = VK_REMAINING_ARRAY_LAYERS;

    [[nodiscard]] static TransitionRequest shader_read(VkPipelineStageFlags2 stage = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT);
    [[nodiscard]] static TransitionRequest transfer_dst();
    [[nodiscard]] static TransitionRequest transfer_src();
    [[nodiscard]] static TransitionRequest color_attachment();
    [[nodiscard]] static TransitionRequest depth_attachment();
    [[nodiscard]] static TransitionRequest present();

    [[nodiscard]] TransitionRequest with_mips(std::uint32_t base, std::uint32_t count) const;
    [[nodiscard]] TransitionRequest with_layers(std::uint32_t base, std::uint32_t count) const;
    [[nodiscard]] TransitionRequest with_queue_family(std::uint32_t family) const;
};

struct Image final {
    struct PerMipState final {
        VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
        std::uint32_t queue_family_index = VK_QUEUE_FAMILY_IGNORED;
        VkAccessFlags2 access = 0;
        VkPipelineStageFlags2 stage = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;

        [[nodiscard]] bool operator==(const PerMipState& other) const;
    };

    struct PerArrayState final {
        std::vector<PerMipState> per_mip_states;

        [[nodiscard]] bool operator==(const PerArrayState& other) const;
    };

    struct State final {
        std::vector<PerArrayState> per_array_states;

        [[nodiscard]] bool is_uniform() const;
        [[nodiscard]] bool has_uniform_layout() const;
    };

    GX_GET_CREF_PRV(std::shared_ptr<memory::Memory>, allocated_memory);
    GX_GET_VAL_PRV(std::uint32_t, image_width, 0);
    GX_GET_VAL_PRV(std::uint32_t, image_height, 0);
    GX_GET_VAL_PRV(std::uint32_t, image_depth, 0);
    GX_GET_VAL_PRV(VkImageType, image_type, VK_IMAGE_TYPE_MAX_ENUM);
    GX_GET_VAL_PRV(std::uint32_t, mipmap_levels, 0);
    GX_GET_VAL_PRV(std::uint32_t, array_layers, 0);
    GX_GET_VAL_PRV(VkFormat, format, VK_FORMAT_UNDEFINED);
    GX_GET_VAL_PRV(VkImageCreateFlags, flags, 0);
    GX_GET_VAL_PRV(VkImageUsageFlags, usage, 0);
    GX_GET_VAL_PRV(VkImage, vulkan_data, nullptr);
    GX_GETSET_VAL_PRV(bool, owned, true);
    GX_GET_CREF_PRV(State, state);

public:
    [[nodiscard]] VkImageAspectFlags get_aspect_flags() const;
    Image(Image&&) = delete;
    Image(const Image&) = delete;
    Image& operator=(Image&&) = delete;
    Image& operator=(const Image&) = delete;
    Image(const std::string& name, std::uint32_t image_width, std::uint32_t image_height, std::uint32_t image_depth, VkImageType image_type, std::uint32_t mipmap_levels, std::uint32_t array_layers, VkFormat format, VkImageCreateFlags flags,
        VkImageUsageFlags usage, VkImage vulkan_data);
    Image(const std::string& name, std::uint32_t image_width, std::uint32_t image_height, std::uint32_t image_depth, VkImageType image_type, std::uint32_t mipmap_levels, std::uint32_t array_layers, VkFormat format, VkImageCreateFlags flags,
        VkImageUsageFlags usage);
    ~Image();
    [[nodiscard]] VkImageLayout get_layout() const;

    /// Unified transition API - transitions to the requested state.
    /// Automatically determines source state from internal tracking.
    /// Updates internal state after transition.
    /// Skips transition if already in the requested state.
    void transit(VkCommandBuffer cmd, const TransitionRequest& request);

    /// Generates mipmaps by blitting from each mip level to the next.
    /// Expects the image to be in TRANSFER_DST layout before calling.
    /// Leaves the entire image in SHADER_READ_ONLY layout when done.
    void generate_mipmaps(VkCommandBuffer cmd);
};
}
#endif
