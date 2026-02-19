#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../core/gx-cr-singleton.hpp"
#include "../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-vk-loader.hpp"

namespace gearoenix::vulkan {
struct Surface final : core::Singleton<Surface> {
    GX_GET_VAL_PRV(VkSurfaceKHR, vulkan_data, nullptr);

public:
    Surface();
    Surface(Surface&&) = delete;
    Surface(const Surface&) = delete;
    Surface& operator=(Surface&&) = delete;
    Surface& operator=(const Surface&) = delete;
    ~Surface() override;
};
}
#endif