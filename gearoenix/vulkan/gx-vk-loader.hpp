#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../core/gx-cr-build-configuration.hpp"

#if GX_PLATFORM_ANDROID
#define VK_USE_PLATFORM_ANDROID_KHR true
#elif GX_PLATFORM_LINUX
#define VK_USE_PLATFORM_XLIB_KHR true
#elif GX_PLATFORM_WINDOWS
#define VK_USE_PLATFORM_WIN32_KHR true
#endif

#if GX_DEBUG_MODE
#define GX_USE_DEBUG_EXTENSIONS true
#endif

#define VK_NO_PROTOTYPES 1
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace gearoenix::vulkan {
struct Loader {
    Loader() = delete;
    [[nodiscard]] static bool is_loaded();
    [[nodiscard]] static bool load();
    static void load(const vk::Instance& instance);
    static void load(const vk::Device& device);
    static void unload();
};
}

#endif
