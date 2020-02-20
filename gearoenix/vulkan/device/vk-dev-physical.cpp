#include "vk-dev-physical.hpp"
#ifdef USE_VULKAN
#include "../../core/cr-static.hpp"
#include "../vk-check.hpp"
#include "../vk-instance.hpp"
#include "../vk-surface.hpp"

int gearoenix::render::device::Physical::is_good(const VkPhysicalDevice& gpu)
{
    auto l = surface->get_instance()->get_linker();
    uint32_t queue_count = 0;
    l->vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_count, NULL);
    if (queue_count == 0) {
        return -1;
    }
    std::vector<VkQueueFamilyProperties> queue_props(queue_count);
    l->vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_count,
        queue_props.data());
    std::vector<VkBool32> supports_present(queue_count);
    for (uint32_t i = 0; i < queue_count; ++i) {
        l->vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, surface->get_vulkan_data(),
            &(supports_present[i]));
    }
    for (uint32_t i = 0; i < queue_count; i++) {
        if (((queue_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) && ((queue_props[i].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0) && ((queue_props[i].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0) && supports_present[i] == VK_TRUE) {
            graphics_queue_node_index = i;
            transfer_queue_node_index = i;
            compute_queue_node_index = i;
            present_queue_node_index = i;
            return 100;
        }
        if ((queue_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
            graphics_queue_node_index = i;
        }
        if ((queue_props[i].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0) {
            transfer_queue_node_index = i;
        }
        if ((queue_props[i].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0) {
            compute_queue_node_index = i;
        }
        if (supports_present[i] == VK_TRUE) {
            present_queue_node_index = i;
        }
    }
    for (uint32_t i = 0; i < queue_count; i++) {
        if (((queue_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) && ((queue_props[i].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0) && supports_present[i] == VK_TRUE) {
            graphics_queue_node_index = i;
            transfer_queue_node_index = i;
            present_queue_node_index = i;
            if (compute_queue_node_index != UINT32_MAX) {
                return 90;
            } else {
                return 50;
            }
        }
    }
    for (uint32_t i = 0; i < queue_count; i++) {
        if (((queue_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) && supports_present[i] == VK_TRUE) {
            graphics_queue_node_index = i;
            present_queue_node_index = i;
            if (compute_queue_node_index != UINT32_MAX && transfer_queue_node_index != UINT32_MAX) {
                return 80;
            } else if (transfer_queue_node_index != UINT32_MAX) {
                return 40;
            } else {
                return 25;
            }
        }
    }
    LOGE(std::string(
        "Separate graphics and presenting queues are not supported yet!"));
    return -1;
}

gearoenix::render::device::Physical::Physical(Surface* surface)
    : surface(surface)
{
    const Instance* instance = surface->get_instance();
    const Linker* l = instance->get_linker();
    uint32_t gpu_count = 0;
    VKC(l->vkEnumeratePhysicalDevices(instance->get_vulkan_data(), &gpu_count,
        0));
    std::vector<VkPhysicalDevice> gpus(gpu_count);
    VKC(l->vkEnumeratePhysicalDevices(instance->get_vulkan_data(), &gpu_count,
        gpus.data()));
    int best_device_index = -1;
    int best_device_point = -1;
    for (uint32_t i = 0; i < gpu_count; ++i) {
        const int device_point = is_good(gpus[i]);
        if (device_point > best_device_point) {
            best_device_index = static_cast<int>(i);
            best_device_point = device_point;
        }
    }
    if (best_device_index > -1 && best_device_point > -1) {
        vulkan_data = gpus[best_device_index];
        LOGI(std::string("Physical device point is: ") + std::to_string(is_good(vulkan_data)));
    } else {
        LOGF(std::string("Physical device minimum requirement is not satisfied."));
    }
    l->vkGetPhysicalDeviceProperties(vulkan_data, &properties);
    l->vkGetPhysicalDeviceFeatures(vulkan_data, &features);
    l->vkGetPhysicalDeviceMemoryProperties(vulkan_data, &memory_properties);
    uint32_t queue_family_count = 0;
    l->vkGetPhysicalDeviceQueueFamilyProperties(vulkan_data, &queue_family_count,
        nullptr);
    if (1 > queue_family_count) {
        LOGF(std::string("queue_family_count value is unexpected."));
    }
    queue_family_properties.resize(queue_family_count);
    l->vkGetPhysicalDeviceQueueFamilyProperties(vulkan_data, &queue_family_count,
        queue_family_properties.data());
    uint32_t ext_count = 0;
    l->vkEnumerateDeviceExtensionProperties(vulkan_data, nullptr, &ext_count,
        nullptr);
    if (ext_count > 0) {
        std::vector<VkExtensionProperties> extensions(ext_count);
        if (l->vkEnumerateDeviceExtensionProperties(
                vulkan_data, nullptr, &ext_count, &extensions.front())
            == VK_SUCCESS) {
            for (auto ext : extensions) {
                supported_extensions.push_back(ext.extensionName);
            }
        }
    }
    LOGI(std::string("Supported extensions are:"));
    for (auto& s : supported_extensions) {
        LOGI(std::string("    ") + s);
    }
    l->vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        vulkan_data, surface->get_vulkan_data(), &surface_capabilities);
}

gearoenix::render::device::Physical::~Physical() { }

const VkPhysicalDeviceMemoryProperties&
gearoenix::render::device::Physical::get_memory_properties() const
{
    return memory_properties;
}

const gearoenix::render::Instance* gearoenix::render::device::Physical::get_instance() const
{
    return surface->get_instance();
}

gearoenix::render::Instance* gearoenix::render::device::Physical::get_instance()
{
    return surface->get_instance();
}

const VkSurfaceCapabilitiesKHR&
gearoenix::render::device::Physical::get_surface_capabilities() const
{

    return surface_capabilities;
}

std::vector<VkSurfaceFormatKHR>
gearoenix::render::device::Physical::get_surface_formats() const
{
    uint32_t count = 0;
    surface->get_instance()->get_linker()->vkGetPhysicalDeviceSurfaceFormatsKHR(
        vulkan_data, surface->get_vulkan_data(), &count, 0);
    std::vector<VkSurfaceFormatKHR> formats(count);
    surface->get_instance()->get_linker()->vkGetPhysicalDeviceSurfaceFormatsKHR(
        vulkan_data, surface->get_vulkan_data(), &count, formats.data());
    return formats;
}

const gearoenix::render::Surface* gearoenix::render::device::Physical::get_surface() const
{
    return surface;
}

const VkPhysicalDevice& gearoenix::render::device::Physical::get_vulkan_data()
    const
{
    return vulkan_data;
}

uint32_t gearoenix::render::device::Physical::get_graphics_queue_node_index()
    const
{
    return graphics_queue_node_index;
}

uint32_t gearoenix::render::device::Physical::get_transfer_queue_node_index()
    const
{
    return transfer_queue_node_index;
}

uint32_t gearoenix::render::device::Physical::get_compute_queue_node_index()
    const
{
    return compute_queue_node_index;
}

uint32_t gearoenix::render::device::Physical::get_present_queue_node_index()
    const
{
    return present_queue_node_index;
}

VkFormat gearoenix::render::device::Physical::get_supported_depth_format()
    const
{
    auto l = surface->get_instance()->get_linker();
    std::vector<VkFormat> depth_formats = {
        VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D16_UNORM_S8_UINT,
        VK_FORMAT_D16_UNORM
    };
    VkFormatProperties format_props;
    for (auto& format : depth_formats) {
        l->vkGetPhysicalDeviceFormatProperties(vulkan_data, format, &format_props);
        if (format_props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            return format;
        }
    }
    LOGF(std::string("Error required depth format not found."));
    return VK_FORMAT_UNDEFINED;
}

uint32_t gearoenix::render::device::Physical::get_memory_type_index(
    uint32_t type_bits, uint32_t properties) const
{
    for (uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i) {
        if ((type_bits & 1) == 1) {
            if (((memory_properties.memoryTypes[i].propertyFlags) & properties) == properties) {
                return i;
            }
        }
        type_bits >>= 1;
    }
    LOGF(std::string("Could not find the requsted memory type."));
}

unsigned int gearoenix::render::device::Physical::get_max_memory_alignment() const
{
    auto& limits = properties.limits;
    return std::max(
        std::max(
            std::max(
                limits.minMemoryMapAlignment,
                limits.minStorageBufferOffsetAlignment),
            std::max(
                limits.minTexelBufferOffsetAlignment,
                limits.minUniformBufferOffsetAlignment)),
        std::max(
            std::max(
                limits.optimalBufferCopyOffsetAlignment,
                limits.optimalBufferCopyRowPitchAlignment),
            limits.bufferImageGranularity));
}

const VkPhysicalDeviceProperties& gearoenix::render::device::Physical::get_properties() const
{
    return properties;
}
#endif
