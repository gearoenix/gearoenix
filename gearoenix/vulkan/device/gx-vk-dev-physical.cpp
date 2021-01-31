#include "gx-vk-dev-physical.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-flagger.hpp"
#include "../../math/gx-math-numeric.hpp"
#include "../gx-vk-check.hpp"
#include "../gx-vk-instance.hpp"
#include "../gx-vk-surface.hpp"

int gearoenix::vulkan::device::Physical::is_good(VkPhysicalDevice gpu) noexcept
{
    std::uint32_t queue_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_count, nullptr);
    if (queue_count == 0) {
        return -1;
    }
    std::vector<VkQueueFamilyProperties> queue_props(queue_count);
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_count,
        queue_props.data());
    std::vector<VkBool32> supports_present(queue_count);
    for (uint32_t i = 0; i < queue_count; ++i) {
        vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, surface.get_vulkan_data(), &(supports_present[i]));
    }
    for (std::uint32_t i = 0; i < queue_count; i++) {
        if (GX_FLAG_CHECK(queue_props[i].queueFlags, VK_QUEUE_GRAPHICS_BIT) && GX_FLAG_CHECK(queue_props[i].queueFlags, VK_QUEUE_TRANSFER_BIT) && GX_FLAG_CHECK(queue_props[i].queueFlags, VK_QUEUE_COMPUTE_BIT) && supports_present[i] == VK_TRUE) {
            graphics_queue_node_index = i;
            transfer_queue_node_index = i;
            compute_queue_node_index = i;
            present_queue_node_index = i;
            return 100;
        }
        if GX_FLAG_CHECK (queue_props[i].queueFlags, VK_QUEUE_GRAPHICS_BIT) {
            graphics_queue_node_index = i;
        }
        if GX_FLAG_CHECK (queue_props[i].queueFlags, VK_QUEUE_TRANSFER_BIT) {
            transfer_queue_node_index = i;
        }
        if GX_FLAG_CHECK (queue_props[i].queueFlags, VK_QUEUE_COMPUTE_BIT) {
            compute_queue_node_index = i;
        }
        if (supports_present[i] == VK_TRUE) {
            present_queue_node_index = i;
        }
    }
    for (std::uint32_t i = 0; i < queue_count; i++) {
        if (GX_FLAG_CHECK(queue_props[i].queueFlags, VK_QUEUE_GRAPHICS_BIT) && GX_FLAG_CHECK(queue_props[i].queueFlags, VK_QUEUE_TRANSFER_BIT) && supports_present[i] == VK_TRUE) {
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
        if (GX_FLAG_CHECK(queue_props[i].queueFlags, VK_QUEUE_GRAPHICS_BIT) && supports_present[i] == VK_TRUE) {
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
    GX_LOG_E("Separate graphics and presenting queues are not supported yet!")
    return -1;
}

gearoenix::vulkan::device::Physical::Physical(const Surface& surf) noexcept
    : surface(surf)
    , properties {}
    , features {}
    , memory_properties {}
{
    const auto gpus = get_available_devices(surface.get_instance().get_vulkan_data());
    int best_device_index = -1;
    int best_device_point = -1;
    for (std::uint32_t i = 0; i < gpus.size(); ++i) {
        vkGetPhysicalDeviceProperties(gpus[i], &properties);
        if (VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU != properties.deviceType)
            continue;
        const auto device_point = is_good(gpus[i]);
        if (device_point > best_device_point) {
            best_device_index = static_cast<int>(i);
            best_device_point = device_point;
        }
    }
    if (-1 == best_device_index) {
        for (std::uint32_t i = 0; i < gpus.size(); ++i) {
            const auto device_point = is_good(gpus[i]);
            if (device_point > best_device_point) {
                best_device_index = static_cast<int>(i);
                best_device_point = device_point;
            }
        }
    }
    if (best_device_index == -1 || best_device_point == -1) {
        GX_LOG_F("Physical device minimum requirement is not satisfied.")
    }
    vulkan_data = gpus[best_device_index];
    GX_LOG_D("Physical device point is: " << is_good(vulkan_data))
    vkGetPhysicalDeviceProperties(vulkan_data, &properties);
    GX_LOG_D("Physical device name is: " << properties.deviceName)
    vkGetPhysicalDeviceFeatures(vulkan_data, &features);
    vkGetPhysicalDeviceMemoryProperties(vulkan_data, &memory_properties);
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(vulkan_data, &queue_family_count, nullptr);
    if (1 > queue_family_count) {
        GX_LOG_F("queue_family_count value is unexpected.")
    }
    queue_family_properties.resize(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(vulkan_data, &queue_family_count, queue_family_properties.data());
    std::uint32_t ext_count = 0;
    vkEnumerateDeviceExtensionProperties(vulkan_data, nullptr, &ext_count, nullptr);
    if (ext_count > 0) {
        std::vector<VkExtensionProperties> extensions(ext_count);
        if (vkEnumerateDeviceExtensionProperties(vulkan_data, nullptr, &ext_count, &extensions.front()) == VK_SUCCESS) {
            for (auto ext : extensions) {
                supported_extensions.emplace(ext.extensionName);
            }
        }
    }
    GX_LOG_D("Supported extensions are:")
    for (auto& s : supported_extensions) {
        GX_LOG_D("    " << s)
    }
    std::uint32_t count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(vulkan_data, surface.get_vulkan_data(), &count, nullptr);
    surface_formats.resize(static_cast<std::size_t>(count));
    vkGetPhysicalDeviceSurfaceFormatsKHR(vulkan_data, surface.get_vulkan_data(), &count, surface_formats.data());
    VkFormatProperties format_props;
    for (auto format : acceptable_depth_formats) {
        vkGetPhysicalDeviceFormatProperties(vulkan_data, format, &format_props);
        if GX_FLAG_CHECK (format_props.optimalTilingFeatures, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            supported_depth_format = format;
            break;
        }
    }
    if (VK_FORMAT_UNDEFINED == supported_depth_format)
        GX_LOG_F("Error required depth format not found.")
    auto& limits = properties.limits;
    max_memory_alignment = static_cast<std::uint32_t>(std::max(
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
            limits.bufferImageGranularity)));
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
    GX_LOG_F("Could not find the requested memory type.")
}

std::size_t gearoenix::vulkan::device::Physical::align_size(const std::size_t size) const noexcept
{
    return math::Numeric::align(size, static_cast<std::size_t>(max_memory_alignment));
}

VkSurfaceCapabilitiesKHR gearoenix::vulkan::device::Physical::get_surface_capabilities() const noexcept
{
    VkSurfaceCapabilitiesKHR info;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vulkan_data, surface.get_vulkan_data(), &info);
    return info;
}

std::vector<VkPhysicalDevice> gearoenix::vulkan::device::Physical::get_available_devices(VkInstance instance) noexcept
{
    std::uint32_t gpu_count = 0;
    GX_VK_CHK(vkEnumeratePhysicalDevices(instance, &gpu_count, nullptr))
    std::vector<VkPhysicalDevice> gpus(static_cast<std::size_t>(gpu_count));
    GX_LOG_D(gpu_count << " GPU(s) are available.")
    GX_VK_CHK(vkEnumeratePhysicalDevices(instance, &gpu_count, gpus.data()))
    return gpus;
}

#endif
