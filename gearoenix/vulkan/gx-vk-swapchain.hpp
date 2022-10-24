#ifndef GEAROENIX_VULKAN_SWAPCHAIN_HPP
#define GEAROENIX_VULKAN_SWAPCHAIN_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-vk-loader.hpp"
#include "image/gx-vk-img-view.hpp"
#include <optional>
#include <vector>

namespace gearoenix::vulkan::device {
struct Logical;
}

namespace gearoenix::vulkan::engine {
struct Engine;
}

namespace gearoenix::vulkan::sync {
struct Semaphore;
}

namespace gearoenix::vulkan {
struct Swapchain final {
    GX_GET_CRRF_PRV(engine::Engine, e);
    GX_GET_CRRF_PRV(device::Logical, logical_device);
    GX_GET_CREF_PRV(VkSurfaceFormatKHR, format);
    GX_GET_VAL_PRV(VkSwapchainKHR, vulkan_data, nullptr);
    GX_GET_CREF_PRV(std::vector<image::View>, image_views);

public:
    Swapchain(const Swapchain&) = delete;
    Swapchain(Swapchain&&) = delete;
    explicit Swapchain(const engine::Engine& e) noexcept;
    ~Swapchain() noexcept;
    Swapchain& operator=(const Swapchain&) = delete;
    Swapchain& operator=(Swapchain&&) = delete;
    /// If the frame is valid it return true otherwise false
    [[nodiscard]] std::optional<std::uint32_t> get_next_image_index(const sync::Semaphore& semaphore) noexcept;
    void initialize() noexcept;
    [[nodiscard]] const VkSwapchainKHR* get_vulkan_data_ptr() const noexcept;
};
}
#endif
#endif
