#ifndef GEAROENIX_VULKAN_IMAGE_VIEW_HPP
#define GEAROENIX_VULKAN_IMAGE_VIEW_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"
#include "gx-vk-img-image.hpp"

namespace gearoenix::vulkan::image {
struct View final {
private:
    GX_GET_CREF_PRV(Image, image)
    GX_GET_VAL_PRV(VkImageView, vulkan_data, nullptr)

public:
    explicit View(Image&& img) noexcept;
    View(View&&) noexcept;
    View(const View&) = delete;
    ~View() noexcept;
    View& operator=(View&&) noexcept;
    View& operator=(const View&) = delete;
    [[nodiscard]] static View create_depth_stencil(memory::Manager& mem_mgr) noexcept;
};
}
#endif
#endif
