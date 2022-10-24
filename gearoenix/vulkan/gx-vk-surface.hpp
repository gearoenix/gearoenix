#ifndef GEAROENIX_VULKAN_SURFACE_HPP
#define GEAROENIX_VULKAN_SURFACE_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-vk-loader.hpp"
#include <memory>

namespace gearoenix::platform {
struct Application;
}

namespace gearoenix::vulkan {
struct Instance;
struct Surface {
    GX_GET_CRRF_PRV(Instance, instance);
    GX_GET_CRRF_PRV(platform::Application, platform_application);
    GX_GET_VAL_PRV(VkSurfaceKHR, vulkan_data, nullptr);

public:
    Surface(const Surface&) = delete;
    Surface(Surface&&) = delete;
    Surface(const Instance& instance, const platform::Application& platform_application) noexcept;
    ~Surface() noexcept;
    Surface& operator=(const Surface&) = delete;
    Surface& operator=(Surface&&) = delete;
};
}
#endif
#endif
