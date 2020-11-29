#ifndef GEAROENIX_VULKAN_INSTANCE_HPP
#define GEAROENIX_VULKAN_INSTANCE_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../core/gx-cr-build-configuration.hpp"
#include "../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-vk-loader.hpp"
#include <memory>
#include <set>

#ifdef GX_DEBUG_MODE
#define GX_VULKAN_INSTANCE_DEBUG
#endif

namespace gearoenix::platform {
struct Application;
}

namespace gearoenix::vulkan {
struct Instance final {
    GX_GET_VAL_PRV(VkInstance, vulkan_data, nullptr)
    GX_GET_CREF_PRV(std::shared_ptr<platform::Application>, platform_application)
#ifdef GX_VULKAN_INSTANCE_DEBUG
    GX_GET_VAL_PRV(VkDebugReportCallbackEXT, report_callback, nullptr)
#endif
public:
    explicit Instance(std::shared_ptr<platform::Application> platform_application) noexcept;
    ~Instance() noexcept;
};
}
#endif
#endif
