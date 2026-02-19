#include "gx-vk-loader.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../platform/gx-plt-log.hpp"
#include "device/gx-vk-dev-logical.hpp"
#include "gx-vk-instance.hpp"

#include <SDL3/SDL_vulkan.h>
#include <cstdlib>
#include <string>

#define GX_VULKAN_LOADER_DECL_FUNCTIONS(GX_VULKAN_LOADER_FUNCTION) PFN_##GX_VULKAN_LOADER_FUNCTION GX_VULKAN_LOADER_FUNCTION = nullptr

GX_VULKAN_FUNCTIONS_MAP(GX_VULKAN_LOADER_DECL_FUNCTIONS)

#undef GX_VULKAN_LOADER_DECL_FUNCTIONS

bool gearoenix::vulkan::Loader::is_loaded() { return vkCreateInstance != nullptr; }

bool gearoenix::vulkan::Loader::load()
{
    if (is_loaded()) {
        return true;
    }

#if GX_PLATFORM_APPLE
    auto loaded = SDL_Vulkan_LoadLibrary("libvulkan.dylib");

    if (!loaded) {
        if (const auto* const env_str = std::getenv("VULKAN_SDK"); env_str != nullptr) {
            const auto lib_path = std::string(env_str) + "/lib/libvulkan.dylib";
            loaded = SDL_Vulkan_LoadLibrary(lib_path.c_str());
        }
    }
#else
    auto loaded = SDL_Vulkan_LoadLibrary(nullptr);
#endif

    if (!loaded) {
        GX_LOG_D("Vulkan library is not available: " << SDL_GetError());
        return false;
    }

    vkGetInstanceProcAddr = reinterpret_cast<decltype(vkGetInstanceProcAddr)>(SDL_Vulkan_GetVkGetInstanceProcAddr());
    if (nullptr == vkGetInstanceProcAddr) {
        GX_LOG_D("Failed to get vkGetInstanceProcAddr: " << SDL_GetError());
        SDL_Vulkan_UnloadLibrary();
        return false;
    }

#define GX_VULKAN_LOADER_LOAD_FUNCTION(GX_VULKAN_LOADER_FUNCTION)                                                                                         \
    if (nullptr == GX_VULKAN_LOADER_FUNCTION) {                                                                                                           \
        GX_VULKAN_LOADER_FUNCTION = reinterpret_cast<PFN_##GX_VULKAN_LOADER_FUNCTION>(vkGetInstanceProcAddr(VK_NULL_HANDLE, #GX_VULKAN_LOADER_FUNCTION)); \
    }

    GX_VULKAN_FUNCTIONS_MAP(GX_VULKAN_LOADER_LOAD_FUNCTION)

#undef GX_VULKAN_LOADER_LOAD_FUNCTION
    return is_loaded();
}

void gearoenix::vulkan::Loader::load([[maybe_unused]] const VkInstance instance)
{
#define GX_VULKAN_LOADER_LOAD_FUNCTION(GX_VULKAN_LOADER_FUNCTION)                                                                                   \
    if (nullptr == GX_VULKAN_LOADER_FUNCTION) {                                                                                                     \
        GX_VULKAN_LOADER_FUNCTION = reinterpret_cast<PFN_##GX_VULKAN_LOADER_FUNCTION>(vkGetInstanceProcAddr(instance, #GX_VULKAN_LOADER_FUNCTION)); \
    }

    GX_VULKAN_FUNCTIONS_MAP(GX_VULKAN_LOADER_LOAD_FUNCTION)

#undef GX_VULKAN_LOADER_LOAD_FUNCTION
}

void gearoenix::vulkan::Loader::load([[maybe_unused]] const VkDevice device)
{
#define GX_VULKAN_LOADER_LOAD_FUNCTION(GX_VULKAN_LOADER_FUNCTION)                                                                               \
    if (nullptr == GX_VULKAN_LOADER_FUNCTION) {                                                                                                 \
        GX_VULKAN_LOADER_FUNCTION = reinterpret_cast<PFN_##GX_VULKAN_LOADER_FUNCTION>(vkGetDeviceProcAddr(device, #GX_VULKAN_LOADER_FUNCTION)); \
    }

    GX_VULKAN_FUNCTIONS_MAP(GX_VULKAN_LOADER_LOAD_FUNCTION)

#undef GX_VULKAN_LOADER_LOAD_FUNCTION
}

void gearoenix::vulkan::Loader::unload() { SDL_Vulkan_UnloadLibrary(); }

PFN_vkVoidFunction gearoenix::vulkan::Loader::get(const char* const name)
{
    auto result = vkGetInstanceProcAddr(VK_NULL_HANDLE, name);
    if (result) {
        return result;
    }

    result = vkGetInstanceProcAddr(Instance::get().get_vulkan_data(), name);
    if (result) {
        return result;
    }

    return vkGetDeviceProcAddr(device::Logical::get().get_vulkan_data(), name);
}

#endif
