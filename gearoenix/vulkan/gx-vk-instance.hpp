#ifndef GEAROENIX_VULKAN_INSTANCE_HPP
#define GEAROENIX_VULKAN_INSTANCE_HPP
#include "../core/gx-cr-build-configuration.hpp"

#ifdef GX_USE_VULKAN
//SDL_Vulkan_GetInstanceExtensions
#include "../core/gx-cr-static.hpp"
#include "gx-vk-loader.hpp"
#include <memory>
#include <set>

#ifdef GX_DEBUG_MODE
#define GX_VULKAN_INSTANCE_DEBUG
#endif

namespace gearoenix::system {
class Application;
}

namespace gearoenix::vulkan {
class Instance final {
    GX_GET_VAL_PRV(VkInstance, vulkan_data, nullptr)
#ifdef GX_VULKAN_INSTANCE_DEBUG
    GX_GET_VAL_PRV(VkDebugReportCallbackEXT, report_callback, nullptr)
#endif
public:
    explicit Instance(system::Application* sys_app) noexcept;
    ~Instance() noexcept;
};
}
#endif
#endif
