#include "gx-vk-surface.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../platform/gx-plt-application.hpp"
#include "gx-vk-check.hpp"
#include "gx-vk-instance.hpp"

#if GX_PLATFORM_INTERFACE_ANDROID
#include <android_native_app_glue.h>
#endif

gearoenix::vulkan::Surface::Surface()
    : Singleton(this)
{
#if GX_PLATFORM_INTERFACE_ANDROID
    VkAndroidSurfaceCreateInfoKHR info;
    GX_SET_ZERO(info);
    info.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
    info.window = platform_application->get_android_app()->window;
    GX_VK_CHK(vkCreateAndroidSurfaceKHR(instance.get_vulkan_data(), &info, nullptr, &vulkan_data));
#elif GX_PLATFORM_INTERFACE_X11
    VkXlibSurfaceCreateInfoKHR info;
    GX_SET_ZERO(info);
    info.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    info.dpy = const_cast<Display*>(platform_application.get_display());
    info.window = platform_application.get_window();
    GX_VK_CHK(vkCreateXlibSurfaceKHR(instance.get_vulkan_data(), &info, nullptr, &vulkan_data));
#elif GX_PLATFORM_INTERFACE_WIN32
    VkWin32SurfaceCreateInfoKHR info;
    GX_SET_ZERO(info);
    info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    info.hinstance = platform_application.get_instance();
    info.hwnd = platform_application.get_window();
    GX_VK_CHK(vkCreateWin32SurfaceKHR(instance.get_vulkan_data(), &info, nullptr, &vulkan_data));
#elif GX_PLATFORM_INTERFACE_SDL
    platform::Application::get().create_vulkan_surface(Instance::get().get_vulkan_data(), &vulkan_data);
#else
#error "Error not implemented yet!"
#endif
}

gearoenix::vulkan::Surface::~Surface()
{
    if (nullptr != vulkan_data) {
        vkDestroySurfaceKHR(Instance::get().get_vulkan_data(), vulkan_data, nullptr);
        vulkan_data = nullptr;
    }
}

#endif
