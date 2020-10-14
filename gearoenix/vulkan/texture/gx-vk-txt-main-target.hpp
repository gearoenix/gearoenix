#ifndef GEAROENIX_VULKAN_TEXTURE_MAIN_TARGET_HPP
#define GEAROENIX_VULKAN_TEXTURE_MAIN_TARGET_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../../render/texture/gx-rnd-txt-target.hpp"

namespace gearoenix::vulkan {
class Framebuffer;
class Instance;
class RenderPass;
class Surface;
class Swapchain;
}

namespace gearoenix::vulkan::engine {
class Engine;
}

namespace gearoenix::vulkan::image {
class View;
}

namespace gearoenix::vulkan::memory {
class Manager;
}

namespace gearoenix::vulkan::sync {
class Fence;
class Semaphore;
}

namespace gearoenix::vulkan::texture {
class MainTarget final : public render::texture::Target {
public:
    struct Frame {
        std::shared_ptr<Framebuffer> framebuffer;
        std::shared_ptr<sync::Fence> wait_fence;
        std::shared_ptr<sync::Semaphore> start_semaphore;
        std::shared_ptr<sync::Semaphore> end_semaphore;

        Frame(
            std::shared_ptr<Framebuffer> framebuffer,
            std::shared_ptr<sync::Fence> wait_fence,
            std::shared_ptr<sync::Semaphore> start_semaphore,
            std::shared_ptr<sync::Semaphore> end_semaphore) noexcept;
    };
    GX_GET_CREF_PRV(std::shared_ptr<Swapchain>, swapchain)
    GX_GET_CREF_PRV(std::shared_ptr<image::View>, depth_stencil)
    GX_GET_CREF_PRV(std::shared_ptr<RenderPass>, render_pass)
    GX_GET_CREF_PRV(std::vector<Frame>, frames)
    GX_GET_PTR_PRV(Frame, current_frame)

    MainTarget(const MainTarget& o) noexcept;

public:
    MainTarget(memory::Manager& memory_manager, engine::Engine* e) noexcept;
    ~MainTarget() noexcept final;
    [[nodiscard]] render::texture::Target* clone() const noexcept final;
    void update() noexcept;
    void present() noexcept;
};
}
#endif
#endif