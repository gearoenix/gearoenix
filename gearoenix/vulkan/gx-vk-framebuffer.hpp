#ifndef GEAROENIX_VULKAN_FRAMEBUFFER_HPP
#define GEAROENIX_VULKAN_FRAMEBUFFER_HPP
#include "../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../core/gx-cr-static.hpp"
#include "gx-vk-loader.hpp"

namespace gearoenix::vulkan::image {
class View;
}

namespace gearoenix::vulkan {
class RenderPass;
class Framebuffer final {
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
