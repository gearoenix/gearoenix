#include "gx-vk-surface.hpp"
#ifdef GX_USE_VULKAN
#include "../core/gx-cr-application.hpp"
#include "../core/gx-cr-static.hpp"
#include "../system/gx-sys-application.hpp"
#include "gx-vk-instance.hpp"

#ifdef GX_IN_ANDROID
#include <android_native_app_glue.h>
#endif

#ifdef GX_USE_SDL
#include <SDL_vulkan.h>
#endif

gearoenix::vulkan::Surface::Surface(std::shared_ptr<Instance> ins, system::Application* const sys_app) noexcept
    : instance(std::move(ins))
    , system_application(sys_app)
{
#ifdef GX_USE_SDL
    if (!SDL_Vulkan_CreateSurface(sys_app->get_window(), instance->get_vulkan_data(), &vulkan_data)) {
        GXLOGF("Unable to create Vulkan Surface from SDL2 system application interface.")
    }
#elif defined(GX_IN_ANDROID)
    VkAndroidSurfaceCreateInfoKHR create_info;
    std::memset(&create_info, 0, sizeof(create_info));
    create_info.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
    create_info.window = sys_app->get_android_app()->window;
    VKC(instance->get_linker()->vkCreateAndroidSurfaceKHR(
        instance->get_vulkan_data(), &create_info, 0, &vulkan_data));
#elif defined(GX_IN_LINUX)
    VkXcbSurfaceCreateInfoKHR create_info;
    GX_SET_ZERO(create_info);
    create_info.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    create_info.connection = sys_app->get_connection();
    create_info.window = sys_app->get_window();
    VKC(instance->get_linker()->vkCreateXcbSurfaceKHR(
        instance->get_vulkan_data(), &create_info, 0, &vulkan_data));
#elif defined(GX_IN_WINDOWS)
    VkWin32SurfaceCreateInfoKHR create_info;
    setz(create_info);
    create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    create_info.hinstance = sys_app->get_hinstance();
    create_info.hwnd = sys_app->get_hwnd();
    VKC(instance->get_linker()->vkCreateWin32SurfaceKHR(
        instance->get_vulkan_data(), &create_info, 0, &vulkan_data));
#else
#error "Error not implemented yet!"
#endif
}

gearoenix::vulkan::Surface::~Surface() noexcept
{
    Loader::vkDestroySurfaceKHR(instance->get_vulkan_data(), vulkan_data, nullptr);
}
#endif
