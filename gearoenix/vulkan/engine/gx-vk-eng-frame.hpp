#ifndef GEAROENIX_VULKAN_ENGINE_FRAME_HPP
#define GEAROENIX_VULKAN_ENGINE_FRAME_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../command/gx-vk-cmd-buffer.hpp"
#include "../command/gx-vk-cmd-manager.hpp"
#include "../gx-vk-framebuffer.hpp"
#include "../gx-vk-swapchain.hpp"
#include "../sync/gx-vk-sync-fence.hpp"
#include "../sync/gx-vk-sync-semaphore.hpp"

namespace gearoenix::vulkan::engine {
struct Frame final {
    command::Buffer draw_command;
    sync::Fence draw_wait;
    Framebuffer framebuffer;
    sync::Semaphore present_complete;
    sync::Semaphore render_complete;

    Frame(const Frame&) = delete;
    Frame(Frame&&) noexcept = default;
    Frame& operator=(const Frame&) = delete;
    Frame& operator=(Frame&&) = delete;

    Frame(
        command::Manager& command_manager,
        const Swapchain&,
        const image::View&,
        const RenderPass&,
        unsigned int) noexcept;

    void begin() noexcept;
};
}

#endif
#endif