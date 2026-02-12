#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../core/gx-cr-build-configuration.hpp"
#include "../core/gx-cr-singleton.hpp"
#include "../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-vk-loader.hpp"

#if GX_DEBUG_MODE
#define GX_VULKAN_INSTANCE_DEBUG true
#endif

namespace gearoenix::vulkan {
struct Instance final : core::Singleton<Instance> {
    GX_GET_CVAL_PRV(std::uint32_t, api_version);
    GX_GET_VAL_PRV(VkInstance, vulkan_data, nullptr);

#if GX_VULKAN_INSTANCE_DEBUG
    GX_GET_VAL_PRV(VkDebugUtilsMessengerEXT, debug_messenger, nullptr);
#endif

    Instance();

public:
    Instance(Instance&&) = delete;
    Instance(const Instance&) = delete;
    Instance& operator=(Instance&&) = delete;
    Instance& operator=(const Instance&) = delete;
    ~Instance() override;

    [[nodiscard]] static std::unique_ptr<Instance> construct();
};
}
#endif