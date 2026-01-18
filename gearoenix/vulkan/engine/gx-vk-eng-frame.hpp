#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include <memory>

namespace gearoenix::vulkan::image {
struct View;
}

namespace gearoenix::vulkan::sync {
struct Semaphore;
}

namespace gearoenix::vulkan {
struct Swapchain;
}

namespace gearoenix::vulkan::engine {
struct Frame final {
    const std::shared_ptr<image::View> view;
    const std::shared_ptr<sync::Semaphore> present_semaphore;
    const std::shared_ptr<sync::Semaphore> end_semaphore;


    Frame(Frame&&) = delete;
    Frame(const Frame&) = delete;
    Frame& operator=(Frame&&) = delete;
    Frame& operator=(const Frame&) = delete;

    Frame(std::shared_ptr<image::View>&& view);
    ~Frame();
    [[nodiscard]] bool present();
};
}

#endif