#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/texture/gx-rnd-txt-target.hpp"

namespace gearoenix::vulkan {
struct Framebuffer;
struct Instance;
struct RenderPass;
struct Surface;
struct Swapchain;
}

namespace gearoenix::vulkan::engine {
struct Engine;
}

namespace gearoenix::vulkan::image {
struct View;
}

namespace gearoenix::vulkan::memory {
struct Manager;
}

namespace gearoenix::vulkan::sync {
struct Fence;
struct Semaphore;
}

namespace gearoenix::vulkan::texture {
struct MainTarget final : render::texture::Target {
    struct Frame {
        std::shared_ptr<Framebuffer> framebuffer;
        std::shared_ptr<sync::Fence> wait_fence;
        std::shared_ptr<sync::Semaphore> start_semaphore;
        std::shared_ptr<sync::Semaphore> end_semaphore;

        Frame(
            std::shared_ptr<Framebuffer> framebuffer,
            std::shared_ptr<sync::Fence> wait_fence,
            std::shared_ptr<sync::Semaphore> start_semaphore,
            std::shared_ptr<sync::Semaphore> end_semaphore);
    };

    GX_GET_CREF_PRV(std::shared_ptr<Swapchain>, swapchain);
    GX_GET_CREF_PRV(std::shared_ptr<image::View>, depth_stencil);
    GX_GET_CREF_PRV(std::shared_ptr<RenderPass>, render_pass);
    GX_GET_CREF_PRV(std::vector<Frame>, frames);
    GX_GET_PTR_PRV(Frame, current_frame);

    MainTarget(const MainTarget& o);

public:
    MainTarget(memory::Manager& memory_manager, engine::Engine* e);
    ~MainTarget() override;
    [[nodiscard]] Target* clone() const override;
    void update();
    void present();
};
}
#endif