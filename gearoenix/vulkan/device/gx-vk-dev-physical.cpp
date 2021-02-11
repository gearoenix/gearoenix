#include "gx-vk-dev-physical.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../../core/macro/gx-cr-mcr-flagger.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
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

void gearoenix::vulkan::device::Physical::initialize_extensions() noexcept
{
    std::uint32_t ext_count = 0;
    GX_VK_CHK(vkEnumerateDeviceExtensionProperties(vulkan_data, nullptr, &ext_count, nullptr))
    if (ext_count <= 0)
        return;
    std::vector<VkExtensionProperties> extensions(static_cast<std::size_t>(ext_count));
    GX_VK_CHK(vkEnumerateDeviceExtensionProperties(vulkan_data, nullptr, &ext_count, extensions.data()))
    for (auto& ext : extensions) {
        supported_extensions.emplace(ext.extensionName);
    }
    if (supported_extensions.contains(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME) && supported_extensions.contains(VK_KHR_RAY_QUERY_EXTENSION_NAME) && supported_extensions.contains(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME) && supported_extensions.contains(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME)) {
        rtx_supported = true;
    }
    GX_LOG_D("Supported extensions are:")
    for (auto& s : supported_extensions) {
        GX_LOG_D("    " << s)
    }
}

void gearoenix::vulkan::device::Physical::initialize_features() noexcept
{
    vkGetPhysicalDeviceFeatures(vulkan_data, &features);
    VkPhysicalDeviceFeatures2 info;
    GX_SET_ZERO(info)
    info.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

    GX_SET_ZERO(ray_query_features)
    ray_query_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR;

    GX_SET_ZERO(ray_tracing_pipeline_features)
    ray_tracing_pipeline_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;

    if (rtx_supported) {
        info.pNext = &ray_query_features;
        vkGetPhysicalDeviceFeatures2(vulkan_data, &info);

        info.pNext = &ray_tracing_pipeline_features;
        vkGetPhysicalDeviceFeatures2(vulkan_data, &info);
    }
}

void gearoenix::vulkan::device::Physical::initialize_properties() noexcept
{
    vkGetPhysicalDeviceProperties(vulkan_data, &properties);
    GX_LOG_D("Physical device name is: " << properties.deviceName)
    vkGetPhysicalDeviceMemoryProperties(vulkan_data, &memory_properties);

    std::uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(vulkan_data, &count, nullptr);
    GX_CHECK_NOT_EQUAL_D(0, count)
    queue_family_properties.resize(count);
    vkGetPhysicalDeviceQueueFamilyProperties(vulkan_data, &count, queue_family_properties.data());

    VkPhysicalDeviceProperties2 info;
    GX_SET_ZERO(info)
    info.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

    GX_SET_ZERO(ray_tracing_pipeline_properties)
    ray_tracing_pipeline_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;

    if (rtx_supported) {
        info.pNext = &ray_tracing_pipeline_properties;
        vkGetPhysicalDeviceProperties2(vulkan_data, &info);
    }
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
    initialize_extensions();
    initialize_features();
    initialize_properties();
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
