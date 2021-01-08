#ifndef GEAROENIX_VULKAN_FRAMEBUFFER_HPP
#define GEAROENIX_VULKAN_FRAMEBUFFER_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-vk-loader.hpp"

namespace gearoenix::vulkan::image {
struct View;
}

namespace gearoenix::vulkan {
struct RenderPass;
struct Framebuffer final {
    GX_GET_REFC_PRV(std::shared_ptr<image::View>, view)
    GX_GET_REFC_PRV(std::shared_ptr<image::View>, depth)
    GX_GET_REFC_PRV(std::shared_ptr<RenderPass>, render_pass)
    GX_GET_VAL_PRV(VkFramebuffer, vulkan_data, nullptr)

public:
    Framebuffer(
        std::shared_ptr<image::View> view,
        std::shared_ptr<image::View> depth,
        std::shared_ptr<RenderPass> render_pass) noexcept;
    ~Framebuffer() noexcept;
};
}
#endif
#endif
