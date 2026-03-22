#include "gx-vk-loader.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../platform/gx-plt-log.hpp"

#include <SDL3/SDL_vulkan.h>
#include <cstdlib>
#include <string>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

bool gearoenix::vulkan::Loader::is_loaded()
{
    return VULKAN_HPP_DEFAULT_DISPATCHER.vkCreateInstance != nullptr;
}

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

    const auto vkGetInstanceProcAddr_fn = reinterpret_cast<PFN_vkGetInstanceProcAddr>(
        SDL_Vulkan_GetVkGetInstanceProcAddr());
    if (nullptr == vkGetInstanceProcAddr_fn) {
        GX_LOG_D("Failed to get vkGetInstanceProcAddr: " << SDL_GetError());
        SDL_Vulkan_UnloadLibrary();
        return false;
    }

    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr_fn);
    return is_loaded();
}

void gearoenix::vulkan::Loader::load(const vk::Instance& instance)
{
    VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);
}

void gearoenix::vulkan::Loader::load(const vk::Device& device)
{
    VULKAN_HPP_DEFAULT_DISPATCHER.init(device);
}

void gearoenix::vulkan::Loader::unload() { SDL_Vulkan_UnloadLibrary(); }

#endif
