#ifndef GEAROENIX_VULKAN_ENGINE_FRAME_HPP
#define GEAROENIX_VULKAN_ENGINE_FRAME_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include <memory>

namespace gearoenix::vulkan::command {
struct Buffer;
struct Manager;
}

namespace gearoenix::vulkan::image {
struct View;
}

namespace gearoenix::vulkan::sync {
struct Fence;
struct Semaphore;
}

namespace gearoenix::vulkan {
struct Framebuffer;
struct Swapchain;
struct RenderPass;
}

namespace gearoenix::vulkan::engine {
struct Frame final {
    const std::unique_ptr<command::Buffer> draw_command;
    const std::unique_ptr<sync::Fence> draw_wait;
    const std::unique_ptr<Framebuffer> framebuffer;
    const std::unique_ptr<sync::Semaphore> present_complete;
    const std::unique_ptr<sync::Semaphore> render_complete;

    Frame(Frame&&) = delete;
    Frame(const Frame&) = delete;
    Frame& operator=(Frame&&) = delete;
    Frame& operator=(const Frame&) = delete;

    Frame(
        command::Manager& command_manager,
        const Swapchain&,
        const image::View&,
        const RenderPass&,
        unsigned int) noexcept;
    ~Frame() noexcept;

    void begin() noexcept;
};
}

#endif
#endif