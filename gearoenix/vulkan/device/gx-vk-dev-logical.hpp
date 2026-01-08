#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/gx-cr-build-configuration.hpp"
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../core/gx-cr-singleton.hpp"
#include "../gx-vk-loader.hpp"

#if GX_DEBUG_MODE
#define GX_VULKAN_DEVICE_DEBUG_MODE true
#endif

namespace gearoenix::vulkan::device {
struct Logical final : core::Singleton<Logical> {
    GX_GET_VAL_PRV(VkDevice, vulkan_data, nullptr);
    GX_GET_VAL_PRV(bool, debug_marker_is_available, false);

public:
    Logical();
    Logical(Logical&&) = delete;
    Logical(const Logical&) = delete;
    Logical& operator=(Logical&&) = delete;
    Logical& operator=(const Logical&) = delete;
    ~Logical() override;

    void wait_to_finish();
};
}
#endif
