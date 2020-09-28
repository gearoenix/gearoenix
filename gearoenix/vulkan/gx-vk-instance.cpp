#include "gx-vk-instance.hpp"
#ifdef GX_USE_VULKAN
#include "../system/gx-sys-application.hpp"
#include "gx-vk-check.hpp"

#ifdef GX_USE_SDL
#include <SDL_vulkan.h>
#endif

#include <set>
#include <sstream>
#include <vector>

namespace gearoenix::vulkan {
#ifdef GX_VULKAN_INSTANCE_DEBUG
static VkBool32 VKAPI_PTR implVkDebugReportCallbackEXT(
    VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType,
    uint64_t object, size_t location, int32_t messageCode,
    const char* pLayerPrefix, const char* pMessage, void* pUserData)
{
    std::stringstream msg;
    msg << "Vulkan ";
    bool is_error = false;
    if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) {
        msg << "Information ";
    }
    if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
        is_error = true;
        msg << "Warning ";
    }
    if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
        is_error = true;
        msg << "Performance ";
    }
    if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
        is_error = true;
        msg << "Error ";
    }
    if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) {
        msg << "Debug ";
    }
    msg << "Message: object_type: ";
    switch (objectType) {
    case VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT:
        msg << "VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT";
        break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT:
        msg << "VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT";
        break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_PHYSICAL_DEVICE_EXT:
        msg << "VK_DEBUG_REPORT_OBJECT_TYPE_PHYSICAL_DEVICE_EXT";
        break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_EXT:
        msg << "VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_EXT";
        break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_QUEUE_EXT:
        msg << "VK_DEBUG_REPORT_OBJECT_TYPE_QUEUE_EXT";
        break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_SEMAPHORE_EXT:
        msg << "VK_DEBUG_REPORT_OBJECT_TYPE_SEMAPHORE_EXT";
        break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_BUFFER_EXT:
        msg << "VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_BUFFER_EXT";
        break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_FENCE_EXT:
        msg << "VK_DEBUG_REPORT_OBJECT_TYPE_FENCE_EXT";
        break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_MEMORY_EXT:
        msg << "VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_MEMORY_EXT";
        break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT:
        msg << "VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT";
        break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT:
        msg << "VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT";
        break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_EVENT_EXT:
        msg << "VK_DEBUG_REPORT_OBJECT_TYPE_EVENT_EXT";
        break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_QUERY_POOL_EXT:
        msg << "VK_DEBUG_REPORT_OBJECT_TYPE_QUERY_POOL_EXT";
        break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_VIEW_EXT:
        msg << "VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_VIEW_EXT";
        break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_VIEW_EXT:
        msg << "VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_VIEW_EXT";
        break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_SHADER_MODULE_EXT:
        msg << "VK_DEBUG_REPORT_OBJECT_TYPE_SHADER_MODULE_EXT";
        break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_CACHE_EXT:
        msg << "VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_CACHE_EXT";
        break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_LAYOUT_EXT:
        msg << "VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_LAYOUT_EXT";
        break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_RENDER_PASS_EXT:
        msg << "VK_DEBUG_REPORT_OBJECT_TYPE_RENDER_PASS_EXT";
        break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_EXT:
        msg << "VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_EXT";
        break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT_EXT:
        msg << "VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT_EXT";
        break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_SAMPLER_EXT:
        msg << "VK_DEBUG_REPORT_OBJECT_TYPE_SAMPLER_EXT";
        break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_POOL_EXT:
        msg << "VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_POOL_EXT";
        break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_EXT:
        msg << "VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_EXT";
        break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_FRAMEBUFFER_EXT:
        msg << "VK_DEBUG_REPORT_OBJECT_TYPE_FRAMEBUFFER_EXT";
        break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_POOL_EXT:
        msg << "VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_POOL_EXT";
        break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_SURFACE_KHR_EXT:
        msg << "VK_DEBUG_REPORT_OBJECT_TYPE_SURFACE_KHR_EXT";
        break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_SWAPCHAIN_KHR_EXT:
        msg << "VK_DEBUG_REPORT_OBJECT_TYPE_SWAPCHAIN_KHR_EXT";
        break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_MAX_ENUM_EXT:
        msg << "VK_DEBUG_REPORT_OBJECT_TYPE_MAX_ENUM_EXT";
        break;
    case VK_DEBUG_REPORT_OBJECT_TYPE_DEBUG_REPORT_EXT:
        msg << "VK_DEBUG_REPORT_OBJECT_TYPE_DEBUG_REPORT_EXT";
        break;
    default:
        msg << "UNKNOWN_TYPE(" << objectType << ")";
    }
    msg << " object: " << object;
    msg << " location: " << location;
    msg << " message-code: " << messageCode;
    msg << " layer-prefix " << pLayerPrefix;
    msg << " pMessage " << pMessage;
    msg << " userdata " << pUserData;
    const auto msg_str = msg.str();
    if (is_error) {
#ifdef GX_DEBUG_MODE
        GXLOGF(msg_str)
#else
        GXLOGE(msg_str)
#endif
    } else {
        GXLOGD(msg_str)
    }
    return VK_FALSE;
}
#endif

static std::uint32_t find_api_version() noexcept
{
    const std::uint32_t min_version = VK_MAKE_VERSION(1, 0, 0);
    std::uint32_t result;
    if (nullptr != Loader::vkEnumerateInstanceVersion) {
        GX_VK_CHK_L(vkEnumerateInstanceVersion(&result))
    }
    result = std::max(result, min_version);
    GXLOGD("Instance version is: " << VK_VERSION_MAJOR(result) << "." << VK_VERSION_MINOR(result))
    return result;
}
}

gearoenix::vulkan::Instance::Instance(system::Application* const sys_app) noexcept
    : system_application(sys_app)
{
    VkApplicationInfo app_info;
    GX_SET_ZERO(app_info)
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.apiVersion = find_api_version();
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pApplicationName = GX_APP_NAME;
    app_info.pEngineName = GX_ENGINE_NAME;
    std::vector<const char*> instance_extensions;
#ifdef GX_USE_SDL
    unsigned int sdl_ext_count = 0;
    GX_CHECK_EQUAL_D(SDL_Vulkan_GetInstanceExtensions(sys_app->get_window(), &sdl_ext_count, nullptr), SDL_TRUE)
    const auto sdl_ext_index = instance_extensions.size();
    instance_extensions.resize(sdl_ext_index + static_cast<std::size_t>(sdl_ext_count));
    GX_CHECK_EQUAL_D(SDL_Vulkan_GetInstanceExtensions(sys_app->get_window(), &sdl_ext_count, &instance_extensions[sdl_ext_index]), SDL_TRUE)
#else
    instance_extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#ifdef GX_IN_ANDROID
    instance_extensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#elif defined(GX_IN_LINUX)
    instance_extensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined(GX_IN_WINDOWS)
    instance_extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#else
#error "Not implemented yet!"
#endif
#endif
#ifdef GX_VULKAN_INSTANCE_DEBUG
    instance_extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    std::uint32_t layers_count = 0;
    Loader::vkEnumerateInstanceLayerProperties(&layers_count, nullptr);
    std::vector<VkLayerProperties> properties(static_cast<std::size_t>(layers_count));
    std::set<std::string> available_layers;
    Loader::vkEnumerateInstanceLayerProperties(&layers_count, properties.data());
    for (const auto& prop : properties) {
        GXLOGD("Available layer: " << prop.layerName)
        available_layers.emplace(prop.layerName);
    }
    std::vector<const char*> instance_layers;
    const auto insert_layer = [&](const char* const layer_name) {
        if (available_layers.contains(layer_name)) {
            instance_layers.emplace_back(layer_name);
        }
    };
    insert_layer("VK_LAYER_KHRONOS_validation");
    for (const char* const layer_name : instance_layers) {
        GXLOGD("Instance layer: " << layer_name)
    }
    for (std::size_t i = 0; i < instance_extensions.size(); ++i) {
        GXLOGD("extension-" << i << ": " << instance_extensions[i])
    }
#endif
    VkInstanceCreateInfo instance_create_info;
    GX_SET_ZERO(instance_create_info)
    instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_create_info.pApplicationInfo = &app_info;
    instance_create_info.enabledExtensionCount = static_cast<std::uint32_t>(instance_extensions.size());
    instance_create_info.ppEnabledExtensionNames = instance_extensions.data();
#ifdef GX_VULKAN_INSTANCE_DEBUG
    instance_create_info.enabledLayerCount = static_cast<uint32_t>(instance_layers.size());
    instance_create_info.ppEnabledLayerNames = instance_layers.data();
#endif
    GX_VK_CHK_L(vkCreateInstance(&instance_create_info, nullptr, &vulkan_data))
    // Loading instance functions
#define VKL(fun_name)                                               \
    if ((Loader::fun_name) == nullptr) {                            \
        Loader::fun_name = reinterpret_cast<PFN_##fun_name>(        \
            Loader::vkGetInstanceProcAddr(vulkan_data, #fun_name)); \
    }
    VKL(vkCreateDebugReportCallbackEXT)
    VKL(vkDestroyDebugReportCallbackEXT)
    VKL(vkDebugReportMessageEXT)

    VkDebugReportCallbackCreateInfoEXT dbg_info;
    GX_SET_ZERO(dbg_info)
    dbg_info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
    dbg_info.flags = VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT | VK_DEBUG_REPORT_INFORMATION_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT;
    dbg_info.pfnCallback = implVkDebugReportCallbackEXT;
    dbg_info.pUserData = this;
    GX_VK_CHK_L(vkCreateDebugReportCallbackEXT(vulkan_data, &dbg_info, nullptr, &report_callback))
}

gearoenix::vulkan::Instance::~Instance() noexcept
{
#ifdef GX_VULKAN_INSTANCE_DEBUG
    Loader::vkDestroyDebugReportCallbackEXT(vulkan_data, report_callback, nullptr);
#endif
    Loader::vkDestroyInstance(vulkan_data, nullptr);
}
#endif
