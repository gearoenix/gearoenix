#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/gx-cr-build-configuration.hpp"
#include "../../core/gx-cr-singleton.hpp"
#include "../gx-vk-loader.hpp"

#if GX_DEBUG_MODE
#define GX_VULKAN_DEVICE_DEBUG_MODE true
#endif

namespace gearoenix::vulkan::device {
struct Logical final : core::Singleton<Logical> {
private:
    vk::raii::Device vulkan_device;
    bool debug_marker_is_available = false;

public:
    Logical();
    Logical(Logical&&) = delete;
    Logical(const Logical&) = delete;
    Logical& operator=(Logical&&) = delete;
    Logical& operator=(const Logical&) = delete;
    ~Logical() override;

    [[nodiscard]] const vk::raii::Device& get_device() const { return vulkan_device; }
    [[nodiscard]] vk::Device get_vulkan_data() const { return *vulkan_device; }
    [[nodiscard]] bool get_debug_marker_is_available() const { return debug_marker_is_available; }

    void wait_to_finish();
};
}
#endif
