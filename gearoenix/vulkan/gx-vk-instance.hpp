#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../core/gx-cr-build-configuration.hpp"
#include "../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-vk-loader.hpp"
#include <memory>
#include <optional>
#include <set>

#if GX_DEBUG_MODE
#define GX_VULKAN_INSTANCE_DEBUG
#endif

namespace gearoenix::platform {
struct Application;
}

namespace gearoenix::vulkan {
struct Instance final {
    GX_GET_VAL_PRV(VkInstance, vulkan_data, nullptr);
#ifdef GX_VULKAN_INSTANCE_DEBUG
    GX_GET_VAL_PRV(VkDebugReportCallbackEXT, report_callback, nullptr);
#endif
private:
    Instance() = default;

public:
    Instance(const Instance&) = delete;
    Instance(Instance&&);
    [[nodiscard]] static std::optional<Instance> construct(const platform::Application* app);
    ~Instance();
    Instance& operator=(const Instance&) = delete;
    Instance& operator=(Instance&&) = delete;
};
}
#endif