#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"

#include <memory>

namespace gearoenix::vulkan::memory {
struct Memory;
}

namespace gearoenix::vulkan::image {
struct Image final {
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
    GX_GETSET_VAL_PRV(VkImageLayout, layout, VK_IMAGE_LAYOUT_UNDEFINED);

public:
    [[nodiscard]] VkImageAspectFlags get_aspect_flags() const;
    Image(Image&&) = delete;
    Image(const Image&) = delete;
    Image& operator=(Image&&) = delete;
    Image& operator=(const Image&) = delete;
    Image(
        const std::string& name,
        std::uint32_t image_width,
        std::uint32_t image_height,
        std::uint32_t image_depth,
        VkImageType image_type,
        std::uint32_t mipmap_levels,
        std::uint32_t array_layers,
        VkFormat format,
        VkImageCreateFlags flags,
        VkImageUsageFlags usage,
        VkImage vulkan_data);
    Image(
        const std::string& name,
        std::uint32_t image_width,
        std::uint32_t image_height,
        std::uint32_t image_depth,
        VkImageType image_type,
        std::uint32_t mipmap_levels,
        std::uint32_t array_layers,
        VkFormat format,
        VkImageCreateFlags flags,
        VkImageUsageFlags usage);
    ~Image();

    /// Transitions the image to the new layout if needed.
    /// Uses the internally tracked layout as the old layout.
    /// Updates the internal layout after transition.
    void transit(VkCommandBuffer cmd, VkImageLayout new_layout);

    /// Transitions a specific mip level range of the image.
    /// This overload is used for per-mip operations like mipmap generation.
    /// Does NOT update the internal layout (caller must manage state for partial transitions).
    void transit_mips(
        VkCommandBuffer cmd,
        VkImageLayout old_layout,
        VkImageLayout new_layout,
        std::uint32_t base_mip_level,
        std::uint32_t level_count) const;
};
}
#endif
