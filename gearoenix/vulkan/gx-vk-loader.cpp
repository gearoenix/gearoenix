#include "gx-vk-loader.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../platform/gx-plt-library.hpp"
#include "../platform/gx-plt-log.hpp"
#include <memory>

#define GX_VULKAN_LOADER_DECL_FUNCTIONS(GX_VULKAN_LOADER_FUNCTION) \
    PFN_##GX_VULKAN_LOADER_FUNCTION GX_VULKAN_LOADER_FUNCTION = nullptr

GX_VULKAN_FUNCTIONS_MAP(GX_VULKAN_LOADER_DECL_FUNCTIONS)

#undef GX_VULKAN_LOADER_DECL_FUNCTIONS

static std::unique_ptr<gearoenix::platform::Library> vulkan_lib = nullptr;

bool gearoenix::vulkan::Loader::is_loaded()
{
    return vkCreateInstance != nullptr;
}

bool gearoenix::vulkan::Loader::load()
{
    if (is_loaded())
        return true;
    vulkan_lib = std::unique_ptr<platform::Library>(platform::Library::construct(
#if GX_PLATFORM_WINDOWS
        "vulkan-1.dll"
#else
        "libvulkan.so"
#endif
        ));

    if (nullptr == vulkan_lib) {
        GX_LOG_D("Vulkan library is not available.");
        return false;
    }

#define GX_VULKAN_LOADER_LOAD_FUNCTION(GX_VULKAN_LOADER_FUNCTION) \
    GX_VULKAN_LOADER_FUNCTION = vulkan_lib->load<PFN_##GX_VULKAN_LOADER_FUNCTION>(#GX_VULKAN_LOADER_FUNCTION)

    GX_VULKAN_FUNCTIONS_MAP(GX_VULKAN_LOADER_LOAD_FUNCTION)

#undef GX_VULKAN_LOADER_LOAD_FUNCTION
    return is_loaded();
}

void gearoenix::vulkan::Loader::load([[maybe_unused]] VkInstance instance)
{
#define GX_VULKAN_LOADER_LOAD_FUNCTION(GX_VULKAN_LOADER_FUNCTION)                      \
    if (nullptr == GX_VULKAN_LOADER_FUNCTION) {                                        \
        GX_VULKAN_LOADER_FUNCTION = reinterpret_cast<PFN_##GX_VULKAN_LOADER_FUNCTION>( \
            vkGetInstanceProcAddr(instance, #GX_VULKAN_LOADER_FUNCTION));              \
    }

    GX_VULKAN_FUNCTIONS_MAP(GX_VULKAN_LOADER_LOAD_FUNCTION)

#undef GX_VULKAN_LOADER_LOAD_FUNCTION
}

void gearoenix::vulkan::Loader::load([[maybe_unused]] VkDevice device)
{
#define GX_VULKAN_LOADER_LOAD_FUNCTION(GX_VULKAN_LOADER_FUNCTION)                      \
    if (nullptr == GX_VULKAN_LOADER_FUNCTION) {                                        \
        GX_VULKAN_LOADER_FUNCTION = reinterpret_cast<PFN_##GX_VULKAN_LOADER_FUNCTION>( \
            vkGetDeviceProcAddr(device, #GX_VULKAN_LOADER_FUNCTION));                  \
    }

    GX_VULKAN_FUNCTIONS_MAP(GX_VULKAN_LOADER_LOAD_FUNCTION)

#undef GX_VULKAN_LOADER_LOAD_FUNCTION
}

void gearoenix::vulkan::Loader::unload()
{
    vulkan_lib = nullptr;
}

PFN_vkVoidFunction gearoenix::vulkan::Loader::get(const char* const name)
{
    return vulkan_lib->load<PFN_vkVoidFunction>(name);
}

#endif
