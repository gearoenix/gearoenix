#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../core/gx-cr-singleton.hpp"
#include "gx-vk-loader.hpp"

namespace gearoenix::vulkan {
struct Surface final : core::Singleton<Surface> {
private:
    vk::raii::SurfaceKHR vulkan_surface;

public:
    Surface();
    Surface(Surface&&) = delete;
    Surface(const Surface&) = delete;
    Surface& operator=(Surface&&) = delete;
    Surface& operator=(const Surface&) = delete;
    ~Surface() override;

    [[nodiscard]] const vk::raii::SurfaceKHR& get_surface() const { return vulkan_surface; }
    [[nodiscard]] vk::SurfaceKHR get_vulkan_data() const { return *vulkan_surface; }
};
}
#endif
