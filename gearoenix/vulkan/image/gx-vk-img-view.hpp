#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"
#include "gx-vk-img-image.hpp"

namespace gearoenix::vulkan::image {
struct View final {
    GX_GET_UCPTR_PRV(Image, image);
    GX_GET_VAL_PRV(VkImageView, vulkan_data, nullptr);
    GX_GET_VAL_PRV(VkImageLayout, layout, VK_IMAGE_LAYOUT_UNDEFINED); // TODO: do the writing
    GX_GET_CREF_PRV(VkExtent3D, extent); // TODO: do the writing

    void terminate();

public:
    explicit View(std::unique_ptr<Image>&& img);
    View(View&&) = delete;
    View(const View&) = delete;
    View& operator=(View&&) = delete;
    View& operator=(const View&) = delete;
    ~View();

    [[nodiscard]] static View create_depth_stencil();
};
}
#endif