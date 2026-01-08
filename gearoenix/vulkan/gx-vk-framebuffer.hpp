#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-vk-loader.hpp"

#include <memory>
#include <vector>

namespace gearoenix::vulkan::image {
struct View;
}

namespace gearoenix::vulkan {
struct RenderPass;
struct Framebuffer final {
    GX_GET_REFC_PRV(std::shared_ptr<image::View>, view);
    GX_GET_REFC_PRV(std::shared_ptr<image::View>, depth);
    GX_GET_REFC_PRV(std::shared_ptr<RenderPass>, render_pass);
    GX_GET_CREF_PRV(std::vector<VkClearValue>, clear_colors);
    GX_GET_CVAL_PRV(VkFramebuffer, vulkan_data);

public:
    Framebuffer(Framebuffer&&) = delete;
    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(Framebuffer&&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;

    Framebuffer(std::shared_ptr<image::View>&& v, std::shared_ptr<image::View>&& d, std::shared_ptr<RenderPass>&& rp);
    ~Framebuffer();
};
}
#endif