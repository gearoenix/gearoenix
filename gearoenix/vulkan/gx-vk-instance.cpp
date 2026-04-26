#include "gx-vk-instance.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../core/gx-cr-application.hpp"
#include "../platform/gx-plt-application.hpp"
#include "../platform/gx-plt-log.hpp"

#include <array>
#include <cstdint>
#include <set>
#include <sstream>
#include <vector>
#include <vulkan/vulkan_to_string.hpp>

namespace gearoenix::vulkan {
#if GX_VULKAN_INSTANCE_DEBUG
static vk::Bool32 VKAPI_PTR impl_vk_debug_utils_callback(
    const vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
    const vk::DebugUtilsMessageTypeFlagsEXT types,
    const vk::DebugUtilsMessengerCallbackDataEXT* const callback_data,
    void* const)
{
    thread_local std::stringstream msg;
    msg.clear();
    msg.str(std::string());

    msg << "Vulkan ";
    if (severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose) {
        msg << "Verbose";
    } else if (severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo) {
        msg << "Info";
    } else if (severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning) {
        msg << "Warning";
    } else {
        msg << "Error";
    }
    msg << " |";
    if (types & vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral) {
        msg << " General";
    }
    if (types & vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation) {
        msg << " Validation";
    }
    if (types & vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance) {
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
            msg << " (" << info.objectHandle << ":" << vk::to_string(info.objectType);
            if (info.pObjectName != nullptr) {
                msg << "-" << info.pObjectName;
            }
            msg << ")";
        }
    }
    const auto msg_str = msg.str();
    if (severity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning) {
        if constexpr (GX_DEBUG_MODE) {
            if (msg_str.contains("VK_LAYER_AMD_switchable_graphics")) {
                return vk::False;
            }
            GX_LOG_F(msg_str);
        } else {
            GX_LOG_E(msg_str);
        }
    } else {
        GX_LOG_D(msg_str);
    }
    return vk::False;
}
#endif

static std::uint32_t find_api_version(const vk::raii::Context& context)
{
    constexpr auto min_version = vk::makeApiVersion(0, 1, 4, 0);
    const auto result = context.enumerateInstanceVersion();
    const auto final_version = std::max(result, min_version);
    GX_LOG_D("Instance version is: " << vk::apiVersionMajor(final_version) << "." << vk::apiVersionMinor(final_version));
    return final_version;
}
}

gearoenix::vulkan::Instance::Instance()
    : Singleton(this)
    , vulkan_instance(nullptr)
#if GX_VULKAN_INSTANCE_DEBUG
    , debug_messenger(nullptr)
#endif
{
}

gearoenix::vulkan::Instance::~Instance() = default;

std::unique_ptr<gearoenix::vulkan::Instance> gearoenix::vulkan::Instance::construct()
{
    std::unique_ptr<Instance> instance(new Instance());
    instance->api_version = find_api_version(instance->context);

    const vk::ApplicationInfo app_info(
        core::Application::get_application_name().c_str(),
        vk::makeApiVersion(0, 1, 0, 0),
        GX_ENGINE_NAME,
        vk::makeApiVersion(0, 1, 0, 0),
        instance->api_version);

    std::set<std::string> instance_extensions_set;
    instance_extensions_set.insert(VK_KHR_SURFACE_EXTENSION_NAME);
    instance_extensions_set.insert(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

#if GX_PLATFORM_APPLE
    instance_extensions_set.insert(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif

    auto sdl_extensions = platform::Application::get().get_vulkan_extensions();
    instance_extensions_set.insert(std::make_move_iterator(sdl_extensions.begin()), std::make_move_iterator(sdl_extensions.end()));

    std::vector<const char*> instance_layers;
#if GX_VULKAN_INSTANCE_DEBUG
    instance_extensions_set.insert(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    const auto available_layers = instance->context.enumerateInstanceLayerProperties();
    std::set<std::string> available_layer_names;
    for (const auto& prop : available_layers) {
        GX_LOG_D("Available layer: " << prop.layerName.data());
        available_layer_names.emplace(prop.layerName.data());
    }
    const auto insert_layer = [&](const char* const layer_name) {
        if (available_layer_names.contains(layer_name)) {
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
    constexpr vk::DebugUtilsMessengerCreateInfoEXT debug_info(
        { },
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose
            | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo
            | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
            | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
            | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
            | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
        impl_vk_debug_utils_callback,
        nullptr);

    // Programmatic equivalent of a `vk_layer_settings.txt` file — configures the
    // Khronos validation layer through `VK_EXT_layer_settings` (a layer-consumed
    // extension attached via pNext). We turn on `VK_DBG_LAYER_ACTION_BREAK` so the
    // layer raises SIGTRAP on every violation (including the threading ones); when
    // run under lldb this halts the process at the exact moment, with all threads
    // still at their offending callsites — `bt all` then shows which path reached
    // the queue without holding `submission_lock`.
    //
    // Arrays are plain locals (non-const) because `vk::ArrayProxyNoTemporaries<T>`
    // needs `data()` convertible to `T*` — a `constexpr std::array<const char*, N>`
    // yields `const char* const*`, which doesn't bind. Local lifetime is fine:
    // everything here outlives the call to `createInstance` below.
    constexpr const char* validation_layer_name = "VK_LAYER_KHRONOS_validation";
    std::array<const char*, 2> debug_action_values {
        "VK_DBG_LAYER_ACTION_LOG_MSG",
        "VK_DBG_LAYER_ACTION_BREAK",
    };
    std::array<const char*, 3> report_flag_values {
        "error",
        "warn",
        "perf",
    };
    constexpr VkBool32 enable_message_limit_value = VK_TRUE;
    constexpr std::uint32_t duplicate_message_limit_value = 1;

    const std::array layer_settings {
        vk::LayerSettingEXT {
            validation_layer_name, "debug_action",
            vk::LayerSettingTypeEXT::eString, debug_action_values },
        vk::LayerSettingEXT {
            validation_layer_name, "report_flags",
            vk::LayerSettingTypeEXT::eString, report_flag_values },
        vk::LayerSettingEXT {
            validation_layer_name, "enable_message_limit",
            vk::LayerSettingTypeEXT::eBool32, 1, &enable_message_limit_value },
        vk::LayerSettingEXT {
            validation_layer_name, "duplicate_message_limit",
            vk::LayerSettingTypeEXT::eUint32, 1, &duplicate_message_limit_value },
    };

    vk::LayerSettingsCreateInfoEXT layer_settings_info(layer_settings);
    // Chain the layer settings behind the debug-utils messenger: InstanceCreateInfo
    //   → DebugUtilsMessengerCreateInfoEXT → LayerSettingsCreateInfoEXT.
    // The constexpr `debug_info` can't have its `pNext` mutated directly, so rebuild
    // a non-const copy that forwards to the settings struct.
    auto chained_debug_info = debug_info;
    chained_debug_info.setPNext(&layer_settings_info);
#endif

    vk::InstanceCreateFlags instance_create_flags;
#if GX_PLATFORM_APPLE
    instance_create_flags = vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;
#endif

    vk::InstanceCreateInfo create_info(
        instance_create_flags,
        &app_info,
        instance_layers,
        instance_extensions);
#if GX_VULKAN_INSTANCE_DEBUG
    create_info.setPNext(&chained_debug_info);
#endif

    try {
        instance->vulkan_instance = instance->context.createInstance(create_info);
    } catch ([[maybe_unused]] const vk::SystemError& e) {
        GX_LOG_D("Failed to create Vulkan instance: " << e.what());
        return nullptr;
    }

    Loader::load(*instance->vulkan_instance);

#if GX_VULKAN_INSTANCE_DEBUG
    instance->debug_messenger = instance->vulkan_instance.createDebugUtilsMessengerEXT(debug_info);
#endif

    return instance;
}

#endif
