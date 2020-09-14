#include "gx-vk-surface.hpp"
#ifdef USE_VULKAN
#include "../core/cr-application.hpp"
#include "../core/cr-build-configuration.hpp"
#include "../core/cr-static.hpp"
#include "../system/sys-app.hpp"
#include "vk-check.hpp"
#include "vk-instance.hpp"
#include <cstring>
#ifdef IN_ANDROID
#include <android_native_app_glue.h>
#endif
gearoenix::render::Surface::Surface(Instance* instance,
    system::Application* sys_app)
    : instance(instance)
    , sys_app(sys_app)
{
#ifdef IN_ANDROID
    VkAndroidSurfaceCreateInfoKHR create_info;
    std::memset(&create_info, 0, sizeof(create_info));
    create_info.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
    create_info.window = sys_app->get_android_app()->window;
    VKC(instance->get_linker()->vkCreateAndroidSurfaceKHR(
        instance->get_vulkan_data(), &create_info, 0, &vulkan_data));
#elif defined(IN_LINUX)
    VkXcbSurfaceCreateInfoKHR create_info;
    setz(create_info);
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

gearoenix::render::Surface::~Surface()
{
    instance->get_linker()->vkDestroySurfaceKHR(instance->get_vulkan_data(),
        vulkan_data, 0);
}

const gearoenix::render::Instance* gearoenix::render::Surface::get_instance() const
{
    return instance;
}

gearoenix::render::Instance* gearoenix::render::Surface::get_instance()
{
    return instance;
}

const VkSurfaceKHR& gearoenix::render::Surface::get_vulkan_data() const
{
    return vulkan_data;
}

const gearoenix::system::Application* gearoenix::render::Surface::get_sys_app()
    const
{
    return sys_app;
}
#endif
