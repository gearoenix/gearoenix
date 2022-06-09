#ifndef GEAROENIX_VULKAN_INSTANCE_HPP
#define GEAROENIX_VULKAN_INSTANCE_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../core/gx-cr-build-configuration.hpp"
#include "../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-vk-loader.hpp"
#include <memory>
#include <optional>
#include <set>

#ifdef GX_DEBUG_MODE
#define GX_VULKAN_INSTANCE_DEBUG
#endif

namespace gearoenix::vulkan {
struct Instance final {
    GX_GET_VAL_PRV(VkInstance, vulkan_data, nullptr)
#ifdef GX_VULKAN_INSTANCE_DEBUG
    GX_GET_VAL_PRV(VkDebugReportCallbackEXT, report_callback, nullptr)
#endif
private:
    Instance() noexcept = default;

public:
    Instance(const Instance&) = delete;
    Instance(Instance&&) noexcept;
    [[nodiscard]] static std::optional<Instance> construct(const char* application_name) noexcept;
    ~Instance() noexcept;
    Instance& operator=(const Instance&) = delete;
    Instance& operator=(Instance&&) = delete;
};
}
#endif
#endif
