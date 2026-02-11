#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../core/gx-cr-singleton.hpp"
#include "../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-vk-loader.hpp"
#include "gx-vk-build-configuration.hpp"

#include <memory>
#include <array>

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
        std::shared_ptr<image::View> imgui_view; // UNORM view for ImGui (null if swapchain is not sRGB)
        std::unique_ptr<sync::Semaphore> present;
    };

    using frames_t = std::array<Frame, frames_in_flight>;

    GX_GET_CREF_PRV(VkSurfaceFormatKHR, format);
    GX_GET_VAL_PRV(VkSwapchainKHR, vulkan_data, nullptr);
    GX_GET_CREF_PRV(frames_t, frames);
    GX_GET_VAL_PRV(bool, is_valid, true);
    GX_GET_VAL_PRV(std::uint32_t, image_index, 0);

public:
    Swapchain();
    Swapchain(Swapchain&&) = delete;
    Swapchain(const Swapchain&) = delete;
    Swapchain& operator=(Swapchain&&) = delete;
    Swapchain& operator=(const Swapchain&) = delete;
    ~Swapchain() override;

    /// If the frame is valid, it returns true otherwise false.
    void acquire_next_image(const sync::Semaphore& semaphore);
    void initialize();
    [[nodiscard]] const VkSwapchainKHR* get_vulkan_data_ptr() const;
    void present();
    [[nodiscard]] const sync::Semaphore& get_present_semaphore() const;
};
}
#endif