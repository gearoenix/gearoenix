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
    GX_GET_REFC_PRV(std::shared_ptr<Instance>, instance)
    GX_GET_CREF_PRV(std::shared_ptr<platform::Application>, platform_application);
    GX_GET_VAL_PRV(VkSurfaceKHR, vulkan_data, nullptr)

public:
    Surface(std::shared_ptr<Instance> instance, std::shared_ptr<platform::Application> platform_application) noexcept;
    ~Surface() noexcept;
};
}
#endif
#endif
