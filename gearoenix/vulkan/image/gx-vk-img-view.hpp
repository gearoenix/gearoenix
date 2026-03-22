#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../gx-vk-loader.hpp"
#include "gx-vk-img-image.hpp"

#include <optional>

namespace gearoenix::vulkan::image {
struct View final {
private:
    std::shared_ptr<Image> image;
    vk::raii::ImageView vulkan_data { nullptr };
    vk::Extent3D extent;
    std::uint32_t base_level = 0;
    std::uint32_t level_count = 1;
    std::uint32_t base_layer = 0;
    std::uint32_t layer_count = 1;

public:
    [[nodiscard]] const std::shared_ptr<Image>& get_image() const { return image; }
    [[nodiscard]] vk::ImageView get_vulkan_data() const { return *vulkan_data; }
    [[nodiscard]] const vk::Extent3D& get_extent() const { return extent; }
    [[nodiscard]] std::uint32_t get_base_level() const { return base_level; }
    [[nodiscard]] std::uint32_t get_level_count() const { return level_count; }
    [[nodiscard]] std::uint32_t get_base_layer() const { return base_layer; }
    [[nodiscard]] std::uint32_t get_layer_count() const { return layer_count; }

    /**
     * @brief Create a new image view.
     * @param img The image to create View from.
     * @param base_level The base mipmap level of the view.
     * @param level_count The number of mipmap levels to include in the view, if it is null, it will use all levels.
     * @param base_layer The base layer of the view.
     * @param layer_count The number of layers to include in the view, if it is null, it will use all layers.
     * @param format_override If provided, overrides the image's format for this view (e.g. UNORM view on sRGB image).
     */
    explicit View(std::shared_ptr<Image>&& img, std::uint32_t base_level = 0, std::optional<std::uint32_t> level_count = std::nullopt, std::uint32_t base_layer = 0, std::optional<std::uint32_t> layer_count = std::nullopt,
        std::optional<vk::Format> format_override = std::nullopt);
    View(View&&) = delete;
    View(const View&) = delete;
    View& operator=(View&&) = delete;
    View& operator=(const View&) = delete;
    ~View();

    [[nodiscard]] static std::shared_ptr<View> create_depth_stencil(const std::string& name);
};
}
#endif
