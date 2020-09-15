#include "gx-vk-dev-physical.hpp"
#ifdef GX_USE_VULKAN
#include "../gx-vk-check.hpp"
#include "../gx-vk-instance.hpp"
#include "../gx-vk-surface.hpp"

int gearoenix::vulkan::device::Physical::is_good(VkPhysicalDevice gpu) noexcept
{
    std::uint32_t queue_count = 0;
    Loader::vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_count, nullptr);
    if (queue_count == 0) {
        return -1;
    }
    std::vector<VkQueueFamilyProperties> queue_props(queue_count);
    Loader::vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_count,
        queue_props.data());
    std::vector<VkBool32> supports_present(queue_count);
    for (uint32_t i = 0; i < queue_count; ++i) {
        Loader::vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, surface->get_vulkan_data(), &(supports_present[i]));
    }
    for (std::uint32_t i = 0; i < queue_count; i++) {
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
    for (std::uint32_t i = 0; i < queue_count; i++) {
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
    GXLOGE("Separate graphics and presenting queues are not supported yet!")
    return -1;
}

gearoenix::vulkan::device::Physical::Physical(std::shared_ptr<Surface> surf) noexcept
    : surface(std::move(surf))
    , properties {}
    , features {}
    , memory_properties {}
    , surface_capabilities {}
{
    const auto& instance = surface->get_instance();
    const auto vk_ins = instance->get_vulkan_data();
    std::uint32_t gpu_count = 0;
    GX_VK_CHK(vkEnumeratePhysicalDevices(vk_ins, &gpu_count, nullptr))
    std::vector<VkPhysicalDevice> gpus(static_cast<std::size_t>(gpu_count));
    GX_VK_CHK(vkEnumeratePhysicalDevices(vk_ins, &gpu_count, gpus.data()))
    int best_device_index = -1;
    int best_device_point = -1;
    for (std::uint32_t i = 0; i < gpu_count; ++i) {
        const auto device_point = is_good(gpus[i]);
        if (device_point > best_device_point) {
            best_device_index = static_cast<int>(i);
            best_device_point = device_point;
        }
    }
    if (best_device_index == -1 || best_device_point == -1) {
        GXLOGF("Physical device minimum requirement is not satisfied.")
    }
    vulkan_data = gpus[best_device_index];
    GXLOGD("Physical device point is: " << is_good(vulkan_data))
    Loader::vkGetPhysicalDeviceProperties(vulkan_data, &properties);
    Loader::vkGetPhysicalDeviceFeatures(vulkan_data, &features);
    Loader::vkGetPhysicalDeviceMemoryProperties(vulkan_data, &memory_properties);
    uint32_t queue_family_count = 0;
    Loader::vkGetPhysicalDeviceQueueFamilyProperties(vulkan_data, &queue_family_count, nullptr);
    if (1 > queue_family_count) {
        GXLOGF("queue_family_count value is unexpected.")
    }
    queue_family_properties.resize(queue_family_count);
    Loader::vkGetPhysicalDeviceQueueFamilyProperties(vulkan_data, &queue_family_count, queue_family_properties.data());
    std::uint32_t ext_count = 0;
    Loader::vkEnumerateDeviceExtensionProperties(vulkan_data, nullptr, &ext_count, nullptr);
    if (ext_count > 0) {
        std::vector<VkExtensionProperties> extensions(ext_count);
        if (Loader::vkEnumerateDeviceExtensionProperties(vulkan_data, nullptr, &ext_count, &extensions.front()) == VK_SUCCESS) {
            for (auto ext : extensions) {
                supported_extensions.emplace_back(ext.extensionName);
            }
        }
    }
    GXLOGD("Supported extensions are:")
    for (auto& s : supported_extensions) {
        GXLOGD("    " << s)
    }
    Loader::vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vulkan_data, surface->get_vulkan_data(), &surface_capabilities);
    std::uint32_t count = 0;
    Loader::vkGetPhysicalDeviceSurfaceFormatsKHR(vulkan_data, surface->get_vulkan_data(), &count, nullptr);
    surface_formats.resize(static_cast<std::size_t>(count));
    Loader::vkGetPhysicalDeviceSurfaceFormatsKHR(vulkan_data, surface->get_vulkan_data(), &count, surface_formats.data());
    std::vector<VkFormat> depth_formats {
        VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D16_UNORM_S8_UINT,
        VK_FORMAT_D16_UNORM
    };
    VkFormatProperties format_props;
    for (auto& format : depth_formats) {
        Loader::vkGetPhysicalDeviceFormatProperties(vulkan_data, format, &format_props);
        if (format_props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            supported_depth_format = format;
            break;
        }
    }
    if (VK_FORMAT_UNDEFINED == supported_depth_format)
        GXLOGF("Error required depth format not found.")
    auto& limits = properties.limits;
    max_memory_alignment = std::max(
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

gearoenix::vulkan::device::Physical::~Physical() noexcept = default;

std::uint32_t gearoenix::vulkan::device::Physical::get_memory_type_index(
    std::uint32_t type_bits, const std::uint32_t mem_properties) const noexcept
{
    for (uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i) {
        if ((type_bits & 1u) == 1u) {
            if (((memory_properties.memoryTypes[i].propertyFlags) & mem_properties) == mem_properties) {
                return i;
            }
        }
        type_bits >>= 1u;
    }
    GXLOGF("Could not find the requested memory type.")
}

#endif
