#include "gx-vk-instance.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../core/gx-cr-application.hpp"
#include "../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../platform/gx-plt-application.hpp"
#include "gx-vk-check.hpp"

#include <set>
#include <sstream>
#include <vector>

namespace gearoenix::vulkan {
#if GX_VULKAN_INSTANCE_DEBUG
static VkBool32 VKAPI_PTR impl_vk_debug_utils_callback(
    const VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    const VkDebugUtilsMessageTypeFlagsEXT types,
    const VkDebugUtilsMessengerCallbackDataEXT* const callback_data,
    void* const)
{
    thread_local std::stringstream msg;
    msg.clear();
    msg.str(std::string());

    msg << "Vulkan ";
    if (severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
        msg << "Verbose";
    } else if (severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        msg << "Info";
    } else if (severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        msg << "Warning";
    } else {
        msg << "Error";
    }
    msg << " |";
    if (types & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
        msg << " General";
    }
    if (types & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
        msg << " Validation";
    }
    if (types & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
        msg << " Performance";
    }
    if (callback_data->pMessageIdName != nullptr) {
        msg << " |" << callback_data->pMessageIdName << "(" << callback_data->messageIdNumber << ")";
    }
    if (callback_data->pMessage != nullptr) {
        msg << " | " << callback_data->pMessage;
    }
    if (callback_data->objectCount > 0) {
        msg << " | Objects:";
        for (std::uint32_t i = 0; i < callback_data->objectCount; ++i) {
            const auto& info = callback_data->pObjects[i];
            msg << " (" << info.objectHandle << ":" << info.objectType;
            if (info.pObjectName != nullptr) {
                msg << "-" << info.pObjectName;
            }
            msg << ")";
        }
    }
    const auto msg_str = msg.str();
    if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        if constexpr (GX_DEBUG_MODE) {
            GX_LOG_F(msg_str);
        } else {
            GX_LOG_E(msg_str);
        }
    } else {
        GX_LOG_D(msg_str);
    }
    return VK_FALSE;
}
#endif

static std::uint32_t find_api_version()
{
    constexpr auto min_version = VK_MAKE_VERSION(1u, 4u, 0u);
    std::uint32_t result = 0;
    if (nullptr != vkEnumerateInstanceVersion) {
        GX_VK_CHK(vkEnumerateInstanceVersion(&result));
    }
    result = std::max(result, min_version);
    GX_LOG_D("Instance version is: " << VK_VERSION_MAJOR(result) << "." << VK_VERSION_MINOR(result));
    return result;
}
}

gearoenix::vulkan::Instance::Instance()
    : Singleton(this)
    , api_version(find_api_version())
{
}

std::unique_ptr<gearoenix::vulkan::Instance> gearoenix::vulkan::Instance::construct()
{
    std::unique_ptr<Instance> instance(new Instance());

    VkApplicationInfo app_info;
    GX_SET_ZERO(app_info);
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.apiVersion = find_api_version();
    app_info.applicationVersion = VK_MAKE_VERSION(1u, 0u, 0u);
    app_info.engineVersion = VK_MAKE_VERSION(1u, 0u, 0u);
    app_info.pApplicationName = core::Application::get_application_name().c_str();
    app_info.pEngineName = GX_ENGINE_NAME;

    std::set<std::string> instance_extensions_set;
    instance_extensions_set.insert(VK_KHR_SURFACE_EXTENSION_NAME);
    instance_extensions_set.insert(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

#if GX_PLATFORM_APPLE
    instance_extensions_set.insert(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif

    auto sdl_extensions = platform::Application::get().get_vulkan_extensions();
    instance_extensions_set.insert(
        std::make_move_iterator(sdl_extensions.begin()),
        std::make_move_iterator(sdl_extensions.end()));

#if GX_VULKAN_INSTANCE_DEBUG
    instance_extensions_set.insert(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    std::uint32_t layers_count = 0;
    vkEnumerateInstanceLayerProperties(&layers_count, nullptr);
    std::vector<VkLayerProperties> properties(static_cast<std::uint64_t>(layers_count));
    std::set<std::string> available_layers;
    vkEnumerateInstanceLayerProperties(&layers_count, properties.data());
    for (const auto& prop : properties) {
        GX_LOG_D("Available layer: " << prop.layerName);
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
        GX_LOG_D("Instance layer: " << layer_name);
    }
    for (const auto& s : instance_extensions_set) {
        GX_LOG_D("extension: " << s);
    }
#endif

    std::vector<const char*> instance_extensions;
    instance_extensions.reserve(instance_extensions_set.size());
    for (const auto& s : instance_extensions_set) {
        instance_extensions.push_back(s.c_str());
    }

#if GX_VULKAN_INSTANCE_DEBUG
    VkDebugUtilsMessengerCreateInfoEXT debug_info;
    GX_SET_ZERO(debug_info);
    debug_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debug_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debug_info.pfnUserCallback = impl_vk_debug_utils_callback;
    debug_info.pUserData = nullptr;
#endif

    VkInstanceCreateInfo instance_create_info;
    GX_SET_ZERO(instance_create_info);
    instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
#if GX_PLATFORM_APPLE
    instance_create_info.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif
    instance_create_info.pApplicationInfo = &app_info;
    instance_create_info.enabledExtensionCount = static_cast<std::uint32_t>(instance_extensions.size());
    instance_create_info.ppEnabledExtensionNames = instance_extensions.data();
#if GX_VULKAN_INSTANCE_DEBUG
    instance_create_info.enabledLayerCount = static_cast<uint32_t>(instance_layers.size());
    instance_create_info.ppEnabledLayerNames = instance_layers.data();
    instance_create_info.pNext = &debug_info;
#endif

    if (const auto create_result = vkCreateInstance(&instance_create_info, nullptr, &instance->vulkan_data); VK_SUCCESS != create_result || nullptr == instance->vulkan_data) {
        return nullptr;
    }

    Loader::load(instance->vulkan_data);

#if GX_VULKAN_INSTANCE_DEBUG
    GX_VK_CHK(vkCreateDebugUtilsMessengerEXT(instance->vulkan_data, &debug_info, nullptr, &instance->debug_messenger));
#endif

    return instance;
}

gearoenix::vulkan::Instance::~Instance()
{
    if (nullptr != vulkan_data) {

#if GX_VULKAN_INSTANCE_DEBUG
        if (nullptr != debug_messenger) {
            vkDestroyDebugUtilsMessengerEXT(vulkan_data, debug_messenger, nullptr);
            debug_messenger = nullptr;
        }
#endif

        vkDestroyInstance(vulkan_data, nullptr);
        vulkan_data = nullptr;
    }
}

#endif
