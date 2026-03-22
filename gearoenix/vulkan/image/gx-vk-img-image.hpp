#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
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
    vk::ImageLayout layout = vk::ImageLayout::eUndefined;
    vk::AccessFlags2 access = { };
    vk::PipelineStageFlags2 stage = vk::PipelineStageFlagBits2::eTopOfPipe;
    std::uint32_t queue_family = vk::QueueFamilyIgnored;
    std::uint32_t base_mip = 0;
    std::uint32_t mip_count = vk::RemainingMipLevels;
    std::uint32_t base_layer = 0;
    std::uint32_t layer_count = vk::RemainingArrayLayers;

    [[nodiscard]] static TransitionRequest shader_read(vk::PipelineStageFlags2 stage = vk::PipelineStageFlagBits2::eFragmentShader);
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
        vk::ImageLayout layout = vk::ImageLayout::eUndefined;
        std::uint32_t queue_family_index = vk::QueueFamilyIgnored;
        vk::AccessFlags2 access = { };
        vk::PipelineStageFlags2 stage = vk::PipelineStageFlagBits2::eTopOfPipe;

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

private:
    std::shared_ptr<memory::Memory> allocated_memory;
    std::uint32_t image_width = 0;
    std::uint32_t image_height = 0;
    std::uint32_t image_depth = 0;
    vk::ImageType image_type = { };
    std::uint32_t mipmap_levels = 0;
    std::uint32_t array_layers = 0;
    vk::Format format = vk::Format::eUndefined;
    vk::ImageCreateFlags flags = { };
    vk::ImageUsageFlags usage = { };
    vk::Image vulkan_data;
    bool owned = true;
    State state;

public:
    [[nodiscard]] const std::shared_ptr<memory::Memory>& get_allocated_memory() const { return allocated_memory; }
    [[nodiscard]] std::uint32_t get_image_width() const { return image_width; }
    [[nodiscard]] std::uint32_t get_image_height() const { return image_height; }
    [[nodiscard]] std::uint32_t get_image_depth() const { return image_depth; }
    [[nodiscard]] vk::ImageType get_image_type() const { return image_type; }
    [[nodiscard]] std::uint32_t get_mipmap_levels() const { return mipmap_levels; }
    [[nodiscard]] std::uint32_t get_array_layers() const { return array_layers; }
    [[nodiscard]] vk::Format get_format() const { return format; }
    [[nodiscard]] vk::ImageCreateFlags get_flags() const { return flags; }
    [[nodiscard]] vk::ImageUsageFlags get_usage() const { return usage; }
    [[nodiscard]] vk::Image get_vulkan_data() const { return vulkan_data; }
    [[nodiscard]] bool get_owned() const { return owned; }
    void set_owned(const bool v) { owned = v; }
    [[nodiscard]] const State& get_state() const { return state; }
    [[nodiscard]] vk::ImageAspectFlags get_aspect_flags() const;

    Image(Image&&) = delete;
    Image(const Image&) = delete;
    Image& operator=(Image&&) = delete;
    Image& operator=(const Image&) = delete;
    Image(const std::string& name, std::uint32_t image_width, std::uint32_t image_height, std::uint32_t image_depth, vk::ImageType image_type, std::uint32_t mipmap_levels, std::uint32_t array_layers, vk::Format format, vk::ImageCreateFlags flags,
        vk::ImageUsageFlags usage, vk::Image vulkan_data);
    Image(const std::string& name, std::uint32_t image_width, std::uint32_t image_height, std::uint32_t image_depth, vk::ImageType image_type, std::uint32_t mipmap_levels, std::uint32_t array_layers, vk::Format format, vk::ImageCreateFlags flags,
        vk::ImageUsageFlags usage);
    ~Image();
    [[nodiscard]] vk::ImageLayout get_layout() const;

    /// Unified transition API - transitions to the requested state.
    /// Automatically determines source state from internal tracking.
    /// Updates internal state after transition.
    /// Skips transition if already in the requested state.
    void transit(vk::CommandBuffer cmd, const TransitionRequest& request);

    /// Generates mipmaps by blitting from each mip level to the next.
    /// Expects the image to be in TRANSFER_DST layout before calling.
    /// Leaves the entire image in the SHADER_READ_ONLY layout when done.
    void generate_mipmaps(vk::CommandBuffer cmd);
};
}
#endif
