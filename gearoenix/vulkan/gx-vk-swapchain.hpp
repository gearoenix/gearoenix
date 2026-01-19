#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../core/gx-cr-singleton.hpp"
#include "../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-vk-loader.hpp"
#include "gx-vk-build-configuration.hpp"

#include <optional>
#include <vector>
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
    using views_t = std::array<std::shared_ptr<image::View>, frames_in_flight>;

    GX_GET_CREF_PRV(VkSurfaceFormatKHR, format);
    GX_GET_VAL_PRV(VkSwapchainKHR, vulkan_data, nullptr);
    GX_GET_CREF_PRV(views_t, image_views);

public:
    Swapchain();
    Swapchain(Swapchain&&) = delete;
    Swapchain(const Swapchain&) = delete;
    Swapchain& operator=(Swapchain&&) = delete;
    Swapchain& operator=(const Swapchain&) = delete;
    ~Swapchain() override;

    /// If the frame is valid, it returns true otherwise false.
    [[nodiscard]] std::optional<std::uint32_t> get_next_image_index(const sync::Semaphore& semaphore);
    void initialize();
    [[nodiscard]] const VkSwapchainKHR* get_vulkan_data_ptr() const;
};
}
#endif