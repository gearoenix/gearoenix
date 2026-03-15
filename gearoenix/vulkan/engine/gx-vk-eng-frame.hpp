#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include <memory>

namespace gearoenix::vulkan::command {
struct Buffer;
}

namespace gearoenix::vulkan::sync {
struct Fence;
struct Semaphore;
}

namespace gearoenix::vulkan::engine {
struct Frame final {
    const std::shared_ptr<sync::Fence> render_fence;
    const std::shared_ptr<sync::Semaphore> present_semaphore;
    const std::shared_ptr<sync::Semaphore> end_semaphore;
    const std::shared_ptr<command::Buffer> cmd;

    Frame(Frame&&) = delete;
    Frame(const Frame&) = delete;
    Frame& operator=(Frame&&) = delete;
    Frame& operator=(const Frame&) = delete;

    explicit Frame(int frame_index);
    ~Frame();
    void start();
};
}

#endif