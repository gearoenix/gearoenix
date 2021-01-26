#ifndef GEAROENIX_VULKAN_FRAMEBUFFER_HPP
#define GEAROENIX_VULKAN_FRAMEBUFFER_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-vk-loader.hpp"
#include <vector>

namespace gearoenix::vulkan::image {
struct View;
}

namespace gearoenix::vulkan {
struct RenderPass;
struct Framebuffer final {
    GX_GET_CPTRC_PRV(image::View, view)
    GX_GET_CPTRC_PRV(image::View, depth)
    GX_GET_CPTRC_PRV(RenderPass, render_pass)
    GX_GET_VAL_PRV(VkFramebuffer, vulkan_data, nullptr)
    GX_GET_CREF_PRV(std::vector<VkClearValue>, clear_colors)

public:
    Framebuffer(const Framebuffer&) = delete;
    Framebuffer(Framebuffer&&) noexcept;
    Framebuffer(const image::View* view, const image::View* depth, const RenderPass* render_pass) noexcept;
    ~Framebuffer() noexcept;
    Framebuffer& operator=(const Framebuffer&) = delete;
    Framebuffer& operator=(Framebuffer&&) = delete;
};
}
#endif
#endif
