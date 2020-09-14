#ifndef GEAROENIX_VULKAN_INSTANCE_HPP
#define GEAROENIX_VULKAN_INSTANCE_HPP
#include "../core/cr-build-configuration.hpp"

#ifdef GX_USE_VULKAN

#include "gx-vk-loader.hpp"
#include <memory>
#include <set>

#ifdef GX_DEBUG_MODE
#define GX_VULKAN_INSTANCE_DEBUG
#endif

namespace gearoenix::vulkan {
class Instance final {
private:
    VkInstance vulkan_data {};
#ifdef GX_VULKAN_INSTANCE_DEBUG
    VkDebugReportCallbackEXT report_callback {};
#endif
public:
    Instance() noexcept;
    ~Instance() noexcept;
    [[nodiscard]] const VkInstance& get_vulkan_data() const noexcept;
};
}
#endif
#endif
