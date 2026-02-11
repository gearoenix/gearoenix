#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include <memory>

namespace gearoenix::vulkan::command {
struct Buffer;
}

namespace gearoenix::vulkan::image {
struct View;
}

namespace gearoenix::vulkan::sync {
    struct Fence;
struct Semaphore;
}

namespace gearoenix::vulkan::engine {
struct Frame final {
    const std::shared_ptr<image::View> view;
    const std::shared_ptr<image::View> imgui_view; // UNORM view for ImGui, may be null
    const std::shared_ptr<sync::Fence> render_fence;
    const std::shared_ptr<sync::Semaphore> present_semaphore;
    const std::shared_ptr<sync::Semaphore> end_semaphore;
    const std::shared_ptr<command::Buffer> cmd;


    Frame(Frame&&) = delete;
    Frame(const Frame&) = delete;
    Frame& operator=(Frame&&) = delete;
    Frame& operator=(const Frame&) = delete;

    Frame(std::shared_ptr<image::View>&& view, std::shared_ptr<image::View>&& imgui_view, int frame_index);
    ~Frame();
};
}

#endif