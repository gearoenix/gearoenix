#ifndef GEAROENIX_VULKAN_ENGINE_FRAME_HPP
#define GEAROENIX_VULKAN_ENGINE_FRAME_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include <memory>

namespace gearoenix::vulkan::image {
struct View;
}

namespace gearoenix::vulkan {
struct Framebuffer;
struct Swapchain;
struct RenderPass;
}

namespace gearoenix::vulkan::engine {
struct Frame final {
    const std::unique_ptr<Framebuffer> framebuffer;

    Frame(Frame&&) = delete;
    Frame(const Frame&) = delete;
    Frame& operator=(Frame&&) = delete;
    Frame& operator=(const Frame&) = delete;

    Frame(
        const Swapchain&,
        const image::View&,
        const RenderPass&,
        unsigned int) noexcept;
    ~Frame() noexcept;
};
}

#endif
#endif