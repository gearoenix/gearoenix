#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../core/gx-cr-singleton.hpp"
#include "gx-vk-build-configuration.hpp"
#include "gx-vk-loader.hpp"

#include <array>
#include <memory>

namespace gearoenix::vulkan::image {
struct View;
}

namespace gearoenix::vulkan::sync {
struct Semaphore;
}

namespace gearoenix::vulkan {
struct Swapchain final : core::Singleton<Swapchain> {
    struct Frame final {
        std::shared_ptr<image::View> view;
        std::unique_ptr<sync::Semaphore> present;
    };

    using frames_t = std::array<Frame, frames_in_flight>;

private:
    vk::SurfaceFormatKHR format { };
    vk::raii::SwapchainKHR vulkan_data { nullptr };
    frames_t frames;
    bool is_valid = true;
    std::uint32_t image_index = 0;

public:
    [[nodiscard]] const vk::SurfaceFormatKHR& get_format() const { return format; }
    [[nodiscard]] vk::SwapchainKHR get_vulkan_data() const { return *vulkan_data; }
    [[nodiscard]] const frames_t& get_frames() const { return frames; }
    [[nodiscard]] bool get_is_valid() const { return is_valid; }
    [[nodiscard]] std::uint32_t get_image_index() const { return image_index; }

    Swapchain();
    Swapchain(Swapchain&&) = delete;
    Swapchain(const Swapchain&) = delete;
    Swapchain& operator=(Swapchain&&) = delete;
    Swapchain& operator=(const Swapchain&) = delete;
    ~Swapchain() override;

    /// If the frame is valid, it returns true otherwise false.
    void acquire_next_image(const sync::Semaphore& semaphore);
    void initialize();
    [[nodiscard]] const Frame& get_frame() const { return frames[image_index]; }
    void present();
    [[nodiscard]] const sync::Semaphore& get_present_semaphore() const;
};
}
#endif