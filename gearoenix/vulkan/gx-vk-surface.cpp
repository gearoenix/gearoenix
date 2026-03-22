#include "gx-vk-surface.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../platform/gx-plt-application.hpp"
#include "gx-vk-instance.hpp"

#if GX_PLATFORM_INTERFACE_ANDROID
#include <android_native_app_glue.h>
#endif

gearoenix::vulkan::Surface::Surface()
    : Singleton(this)
    , vulkan_surface(nullptr)
{
    const auto& instance = Instance::get();
#if GX_PLATFORM_INTERFACE_ANDROID
    const vk::AndroidSurfaceCreateInfoKHR info({ }, platform_application->get_android_app()->window);
    vulkan_surface = instance.get_instance().createAndroidSurfaceKHR(info);
#elif GX_PLATFORM_INTERFACE_X11
    const vk::XlibSurfaceCreateInfoKHR info(
        { },
        const_cast<Display*>(platform_application.get_display()),
        platform_application.get_window());
    vulkan_surface = instance.get_instance().createXlibSurfaceKHR(info);
#elif GX_PLATFORM_INTERFACE_WIN32
    const vk::Win32SurfaceCreateInfoKHR info({ }, platform_application.get_instance(), platform_application.get_window());
    vulkan_surface = instance.get_instance().createWin32SurfaceKHR(info);
#elif GX_PLATFORM_INTERFACE_SDL
    VkSurfaceKHR raw_surface = VK_NULL_HANDLE;
    platform::Application::get().create_vulkan_surface(
        static_cast<VkInstance>(instance.get_vulkan_data()), &raw_surface);
    vulkan_surface = vk::raii::SurfaceKHR(instance.get_instance(), raw_surface);
#else
#error "Error not implemented yet!"
#endif
}

gearoenix::vulkan::Surface::~Surface() = default;

#endif
