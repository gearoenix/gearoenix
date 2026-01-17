#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"
#include "gx-vk-img-image.hpp"

#include <optional>

namespace gearoenix::vulkan::image {
struct View final {
    GX_GET_CREF_PRV(std::shared_ptr<Image>, image);
    GX_GET_VAL_PRV(VkImageView, vulkan_data, nullptr);
    GX_GET_VAL_PRV(VkImageLayout, layout, VK_IMAGE_LAYOUT_UNDEFINED);
    GX_GET_CREF_PRV(VkExtent3D, extent);
    GX_GET_VAL_PRV(std::uint32_t, base_level, 0);
    GX_GET_VAL_PRV(std::uint32_t, level_count, 1);
    GX_GET_VAL_PRV(std::uint32_t, base_layer, 0);
    GX_GET_VAL_PRV(std::uint32_t, layer_count, 1);

public:
    /**
     * @brief Create a new image view.
     * @param img The image to create View from.
     * @param base_level The base mipmap level of the view.
     * @param level_count The number of mipmap levels to include in the view, if it is null, it will use all levels.
     * @param base_layer The base layer of the view.
     * @param layer_count The number of layers to include in the view, if it is null, it will use all layers.
     */
    explicit View(
        std::shared_ptr<Image>&& img,
        std::uint32_t base_level = 0,
        std::optional<std::uint32_t> level_count = std::nullopt,
        std::uint32_t base_layer = 0,
        std::optional<std::uint32_t> layer_count = std::nullopt);
    View(View&&) = delete;
    View(const View&) = delete;
    View& operator=(View&&) = delete;
    View& operator=(const View&) = delete;
    ~View();

    [[nodiscard]] static std::shared_ptr<View> create_depth_stencil();
};
}
#endif