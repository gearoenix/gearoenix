#include "vk-instance.hpp"
#include "vk-check.hpp"
#include <vector>
#include <sstream>
#include "../core/static.hpp"
#ifdef VULKAN_INSTANCE_DEBUG
static VkBool32 VKAPI_PTR vkDebugReportCallbackEX_impl(
        VkDebugReportFlagsEXT flags,
        VkDebugReportObjectTypeEXT objectType,
        uint64_t object,
        size_t location,
        int32_t messageCode,
        const char *pLayerPrefix,
        const char *pMessage,
        void *pUserData) {
    std::stringstream msg;
	msg << "Vulkan ";
    if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) {
		msg << "Information";
    }
    if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
		msg << "Warning";
    }
    if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
		msg << "Performance";
    }
    if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
		msg << "Error";
    }
    if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) {
		msg << "Debug";
    }
	msg << " Message: object_type: ";
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
        case VK_DEBUG_REPORT_OBJECT_TYPE_RANGE_SIZE_EXT:
			msg << "VK_DEBUG_REPORT_OBJECT_TYPE_RANGE_SIZE_EXT";
            break;
        case VK_DEBUG_REPORT_OBJECT_TYPE_MAX_ENUM_EXT:
			msg << "VK_DEBUG_REPORT_OBJECT_TYPE_MAX_ENUM_EXT";
            break;
        case VK_DEBUG_REPORT_OBJECT_TYPE_DEBUG_REPORT_EXT:
			msg << "VK_DEBUG_REPORT_OBJECT_TYPE_MAX_ENUM_EXT";
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
    auto smsg = msg.str();
	if((flags & (VK_DEBUG_REPORT_INFORMATION_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT)) != 0) {
        LOGI(smsg);
    }
	if((flags & (VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT)) != 0) {
        LOGE(smsg);
    }
    return VK_FALSE;
}
#endif

gearoenix::nufrag::render::Instance::Instance(const std::shared_ptr<Linker> &l): linker(l) {
    VkApplicationInfo app_info;
    std::memset(&app_info, 0, sizeof(app_info));
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.apiVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pApplicationName = "Seeki Walki";
    app_info.pEngineName = "NuFrag";
    const char *instance_extensions[] = {
            VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef IN_ANDROID
			VK_KHR_ANDROID_SURFACE_EXTENSION_NAME,
#elif defined(IN_LINUX)
            VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#elif defined(IN_WINDOWS)
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#else
#error "Not implemented yet!"
#endif
#ifdef VULKAN_INSTANCE_DEBUG
            VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
#endif
    };
#ifdef VULKAN_INSTANCE_DEBUG
    uint32_t layers_count = 0;
    l->vkEnumerateInstanceLayerProperties(&layers_count, nullptr);
    int property_count = layers_count;
    std::vector<VkLayerProperties> properties(property_count);
    std::vector<const char *> instance_layers(property_count);
    l->vkEnumerateInstanceLayerProperties(&layers_count, properties.data());
    for (int i = 0; i < property_count; ++i) {
        LOGI(std::string(" layer-") + std::to_string(i) + ":  " +
             std::string(properties[i].layerName));
        instance_layers[i] = properties[i].layerName;
    }
    const char *google_threading = "VK_LAYER_GOOGLE_threading";
    const char *google_unique = "VK_LAYER_GOOGLE_unique_objects";
	const std::string lunarg_trace("VK_LAYER_LUNARG_vktrace");
	const std::string lunarg_api_dump("VK_LAYER_LUNARG_api_dump");
	//const std::string renderdoc_capture("VK_LAYER_RENDERDOC_Capture");
	//const std::string lunarg_screenshot("VK_LAYER_LUNARG_screenshot");
	//const std::string lunarg_monitor("VK_LAYER_LUNARG_monitor");
	// removing annoying layers
	for (auto iter = instance_layers.begin(); iter != instance_layers.end();) {
		const std::string layer_name(*iter);
		if (layer_name == lunarg_trace 
            || layer_name == lunarg_api_dump
			//|| layer_name == renderdoc_capture 
			//|| layer_name == lunarg_screenshot 
			//|| layer_name == lunarg_monitor
			) {
			iter = instance_layers.erase(iter);
		} else {
			++iter;
		}
	}
	// ordering
	for(unsigned int i = 1; i < instance_layers.size() - 1; ++i) {
		const std::string layer_name(instance_layers[i]);
		if (layer_name == google_threading) {
            instance_layers[i] = instance_layers[0];
            instance_layers[0] = google_threading;
        } else if (layer_name == google_unique) {
            instance_layers[i] = instance_layers[instance_layers.size() - 1];
			instance_layers[instance_layers.size() - 1] = google_unique;
		}
    }
    LOGI(std::string("Layers order:"));
	for(unsigned int i = 0; i < instance_layers.size(); ++i) {
        LOGI(std::string("    layer-") + std::to_string(i) + ":  " +
             std::string(instance_layers[i]));
    }
#endif
    VkInstanceCreateInfo instance_create_info;
    setz(instance_create_info);
    instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_create_info.pApplicationInfo = &app_info;
    instance_create_info.enabledExtensionCount = countof(instance_extensions);
    instance_create_info.ppEnabledExtensionNames = instance_extensions;
#ifdef VULKAN_INSTANCE_DEBUG
    instance_create_info.enabledLayerCount = static_cast<uint32_t>(instance_layers.size());
    instance_create_info.ppEnabledLayerNames = instance_layers.data();
#endif
    VKC(l->vkCreateInstance(&instance_create_info, 0, &vulkan_data));
#define VKL(fun_name) if((l->fun_name) == 0){l->fun_name = reinterpret_cast<PFN_##fun_name>(l->vkGetInstanceProcAddr(vulkan_data, #fun_name));}
	VKL(vkCreateDebugReportCallbackEXT);
	VKL(vkDestroyDebugReportCallbackEXT);
	VKL(vkDebugReportMessageEXT);
    VkDebugReportCallbackCreateInfoEXT dbg_info;
    setz(dbg_info);
    dbg_info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
	dbg_info.flags = VK_DEBUG_REPORT_WARNING_BIT_EXT
			| VK_DEBUG_REPORT_DEBUG_BIT_EXT
			| VK_DEBUG_REPORT_INFORMATION_BIT_EXT
			| VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT
			| VK_DEBUG_REPORT_ERROR_BIT_EXT;
    dbg_info.pfnCallback = vkDebugReportCallbackEX_impl;
    dbg_info.pUserData = this;
    VKC(l->vkCreateDebugReportCallbackEXT(vulkan_data, &dbg_info, nullptr, &report_callback));
}

gearoenix::nufrag::render::Instance::~Instance() {
#ifdef VULKAN_INSTANCE_DEBUG
	linker->vkDestroyDebugReportCallbackEXT(vulkan_data, report_callback, nullptr);
#endif
    linker->vkDestroyInstance(vulkan_data, 0);
}

const std::shared_ptr<gearoenix::nufrag::render::Linker> &
gearoenix::nufrag::render::Instance::get_linker() const {
    return linker;
}

const VkInstance &gearoenix::nufrag::render::Instance::get_vulkan_data() const {
    return vulkan_data;
}
