#include "gx-vk-surface.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../platform/gx-plt-application.hpp"
#include "gx-vk-check.hpp"
#include "gx-vk-instance.hpp"

#ifdef GX_PLATFORM_ANDROID
#include <android_native_app_glue.h>
#endif

gearoenix::vulkan::Surface::Surface(const Instance& ins, const platform::Application& plt_app) noexcept
    : instance(ins)
    , platform_application(plt_app)
{
#if defined(GX_PLATFORM_ANDROID)
    VkAndroidSurfaceCreateInfoKHR info;
    GX_SET_ZERO(info)
    info.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
    info.window = platform_application->get_android_app()->window;
    GX_VK_CHK_L(vkCreateAndroidSurfaceKHR(instance.get_vulkan_data(), &info, nullptr, &vulkan_data))
#elif defined(GX_PLATFORM_LINUX)
    VkXlibSurfaceCreateInfoKHR info;
    GX_SET_ZERO(info)
    info.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    info.dpy = const_cast<Display*>(platform_application.get_display());
    info.window = platform_application.get_window();
    GX_VK_CHK_L(vkCreateXlibSurfaceKHR(instance.get_vulkan_data(), &info, nullptr, &vulkan_data))
#elif defined(GX_PLATFORM_WINDOWS)
    VkWin32SurfaceCreateInfoKHR info;
    GX_SET_ZERO(info)
    info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    info.hinstance = platform_application.get_instance();
    info.hwnd = platform_application.get_window();
    GX_VK_CHK_L(vkCreateWin32SurfaceKHR(instance.get_vulkan_data(), &info, nullptr, &vulkan_data))
#else
#error "Error not implemented yet!"
#endif
}

gearoenix::vulkan::Surface::~Surface() noexcept
{
    Loader::vkDestroySurfaceKHR(instance.get_vulkan_data(), vulkan_data, nullptr);
}
#endif
