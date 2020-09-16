#ifndef GEAROENIX_VULKAN_SWAPCHAIN_HPP
#define GEAROENIX_VULKAN_SWAPCHAIN_HPP
#include "../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../core/gx-cr-static.hpp"
#include "gx-vk-loader.hpp"
#include <memory>
#include <vector>

namespace gearoenix::vulkan::device {
class Logical;
}

namespace gearoenix::vulkan::image {
class View;
}

namespace gearoenix::vulkan::sync {
class Semaphore;
}

namespace gearoenix::vulkan {
class Swapchain final {
    GX_GET_REFC_PRV(std::shared_ptr<device::Logical>, logical_device)
    GX_GET_CREF_PRV(VkSurfaceFormatKHR, format)
    GX_GET_VAL_PRV(VkSwapchainKHR, vulkan_data, nullptr)
    GX_GET_CREF_PRV(std::vector<std::shared_ptr<image::View>>, image_views)

public:
    explicit Swapchain(std::shared_ptr<device::Logical> d) noexcept;
    ~Swapchain() noexcept;
    [[nodiscard]] std::uint32_t get_next_image_index(const std::shared_ptr<sync::Semaphore>& semaphore) noexcept;
    void initialize() noexcept;
};
}
#endif
#endif
