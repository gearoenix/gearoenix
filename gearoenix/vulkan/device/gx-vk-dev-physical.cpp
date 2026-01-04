#include "gx-vk-dev-physical.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../../core/macro/gx-cr-mcr-flagger.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../../math/gx-math-numeric.hpp"
#include "../gx-vk-check.hpp"
#include "../gx-vk-instance.hpp"
#include "../gx-vk-surface.hpp"

#include <algorithm>

int gearoenix::vulkan::device::Physical::is_good(const VkPhysicalDevice gpu)
{
    graphics_queue_node_index = std::numeric_limits<std::uint32_t>::max();
    transfer_queue_node_index = std::numeric_limits<std::uint32_t>::max();
    compute_queue_node_index = std::numeric_limits<std::uint32_t>::max();
    present_queue_node_index = std::numeric_limits<std::uint32_t>::max();

    VkPhysicalDeviceProperties candidate_properties;
    vkGetPhysicalDeviceProperties(gpu, &candidate_properties);
    if (VK_VERSION_MAJOR(candidate_properties.apiVersion) < 1 ||
        (VK_VERSION_MAJOR(candidate_properties.apiVersion) == 1 && VK_VERSION_MINOR(candidate_properties.apiVersion) < 3)) {
        GX_LOG_D("Skipping GPU " << candidate_properties.deviceName << " because it does not meet the Vulkan 1.3 requirement.");
        return -1;
    }

    VkPhysicalDeviceFeatures2 features_info;
    GX_SET_ZERO(features_info);
    features_info.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

    VkPhysicalDeviceVulkan12Features v12_features;
    GX_SET_ZERO(v12_features);
    v12_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;

    VkPhysicalDeviceVulkan13Features v13_features;
    GX_SET_ZERO(v13_features);
    v13_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;

    VkPhysicalDeviceDynamicRenderingFeatures dynamic_features;
    GX_SET_ZERO(dynamic_features);
    dynamic_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;

    v13_features.pNext = &dynamic_features;
    v12_features.pNext = &v13_features;
    features_info.pNext = &v12_features;

    vkGetPhysicalDeviceFeatures2(gpu, &features_info);

    const bool dynamic_supported = v13_features.dynamicRendering == VK_TRUE || dynamic_features.dynamicRendering == VK_TRUE;
    if (!dynamic_supported) {
        GX_LOG_D("Skipping GPU " << candidate_properties.deviceName << " because dynamic rendering is not supported.");
        return -1;
    }

    const bool descriptor_indexing_supported =
        v12_features.descriptorIndexing == VK_TRUE &&
        v12_features.runtimeDescriptorArray == VK_TRUE &&
        v12_features.descriptorBindingPartiallyBound == VK_TRUE &&
        v12_features.descriptorBindingSampledImageUpdateAfterBind == VK_TRUE &&
        v12_features.descriptorBindingVariableDescriptorCount == VK_TRUE &&
        v12_features.shaderSampledImageArrayNonUniformIndexing == VK_TRUE;
    if (!descriptor_indexing_supported) {
        GX_LOG_D("Skipping GPU " << candidate_properties.deviceName << " because required descriptor indexing features are missing.");
        return -1;
    }

    std::uint32_t queue_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_count, nullptr);
    if (queue_count == 0) {
        return -1;
    }
    std::vector<VkQueueFamilyProperties> queue_props(queue_count);
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_count, queue_props.data());

    std::vector<VkBool32> supports_present(queue_count);
    for (uint32_t i = 0; i < queue_count; ++i) {
        vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, Surface::get().get_vulkan_data(), &supports_present[i]);
    }

    std::uint32_t graphics_candidate = std::numeric_limits<std::uint32_t>::max();
    std::uint32_t transfer_candidate = std::numeric_limits<std::uint32_t>::max();
    std::uint32_t compute_candidate = std::numeric_limits<std::uint32_t>::max();
    std::uint32_t present_candidate = std::numeric_limits<std::uint32_t>::max();

    const auto commit_and_return = [&](const std::uint32_t g, const std::uint32_t t, const std::uint32_t c, const std::uint32_t p, const int s) {
        graphics_queue_node_index = g;
        transfer_queue_node_index = t;
        compute_queue_node_index = c;
        present_queue_node_index = p;
        return s;
    };

    for (std::uint32_t i = 0; i < queue_count; ++i) {
        const auto qf = queue_props[i].queueFlags;
        const auto sp = supports_present[i];

        if (GX_FLAG_CHECK(qf, VK_QUEUE_GRAPHICS_BIT) && GX_FLAG_CHECK(qf, VK_QUEUE_TRANSFER_BIT) && GX_FLAG_CHECK(qf, VK_QUEUE_COMPUTE_BIT) && sp == VK_TRUE) {
            return commit_and_return(i, i, i, i, 100);
        }
        if GX_FLAG_CHECK (qf, VK_QUEUE_GRAPHICS_BIT) {
            graphics_candidate = i;
        }
        if GX_FLAG_CHECK (qf, VK_QUEUE_TRANSFER_BIT) {
            transfer_candidate = i;
        }
        if GX_FLAG_CHECK (qf, VK_QUEUE_COMPUTE_BIT) {
            compute_candidate = i;
        }
        if (sp == VK_TRUE) {
            present_candidate = i;
        }
    }

    for (std::uint32_t i = 0; i < queue_count; ++i) {
        const auto qf = queue_props[i].queueFlags;
        const auto sp = supports_present[i];

        if (GX_FLAG_CHECK(qf, VK_QUEUE_GRAPHICS_BIT) && GX_FLAG_CHECK(qf, VK_QUEUE_TRANSFER_BIT) && sp == VK_TRUE) {
            graphics_candidate = i;
            transfer_candidate = i;
            present_candidate = i;
            if (compute_candidate != UINT32_MAX) {
                return commit_and_return(graphics_candidate, transfer_candidate, compute_candidate, present_candidate, 90);
            }
            return commit_and_return(graphics_candidate, transfer_candidate, compute_candidate, present_candidate, 50);
        }
    }

    for (uint32_t i = 0; i < queue_count; ++i) {
        const auto qf = queue_props[i].queueFlags;
        const auto sp = supports_present[i];

        if (GX_FLAG_CHECK(qf, VK_QUEUE_GRAPHICS_BIT) && sp == VK_TRUE) {
            graphics_candidate = i;
            present_candidate = i;
            if (compute_candidate != UINT32_MAX && transfer_candidate != UINT32_MAX) {
                return commit_and_return(graphics_candidate, transfer_candidate, compute_candidate, present_candidate, 80);
            }
            if (transfer_candidate != UINT32_MAX) {
                return commit_and_return(graphics_candidate, transfer_candidate, compute_candidate, present_candidate, 40);
            }
            return commit_and_return(graphics_candidate, transfer_candidate, compute_candidate, present_candidate, 25);
        }
    }
    GX_LOG_E("Separate graphics and presenting queues are not supported yet!");
    return -1;
}

void gearoenix::vulkan::device::Physical::initialize_extensions()
{
    std::uint32_t ext_count = 0;
    GX_VK_CHK(vkEnumerateDeviceExtensionProperties(vulkan_data, nullptr, &ext_count, nullptr));
    if (ext_count <= 0) {
        return;
    }
    std::vector<VkExtensionProperties> extensions(static_cast<std::uint64_t>(ext_count));
    GX_VK_CHK(vkEnumerateDeviceExtensionProperties(vulkan_data, nullptr, &ext_count, extensions.data()));
    for (const auto& [extension_name, _] : extensions) {
        supported_extensions.emplace(extension_name);
    }

    if (supported_extensions.contains(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME) &&
        supported_extensions.contains(VK_KHR_RAY_QUERY_EXTENSION_NAME) &&
        supported_extensions.contains(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME) &&
        supported_extensions.contains(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME)) {
        rtx_supported = true;
    }

    GX_LOG_D("Supported extensions are:");
    for (auto& s : supported_extensions) {
        GX_LOG_D("    " << s);
    }
}

void gearoenix::vulkan::device::Physical::initialize_features()
{
    vkGetPhysicalDeviceFeatures(vulkan_data, &features);
    VkPhysicalDeviceFeatures2 info;
    GX_SET_ZERO(info);
    info.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

    GX_SET_ZERO(vulkan_12_features);
    vulkan_12_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;

    GX_SET_ZERO(vulkan_13_features);
    vulkan_13_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;

    GX_SET_ZERO(dynamic_rendering_features);
    dynamic_rendering_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;

    GX_SET_ZERO(shader_clock_features);
    shader_clock_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CLOCK_FEATURES_KHR;

    GX_SET_ZERO(ray_query_features);
    ray_query_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR;
    ray_query_features.pNext = &shader_clock_features;

    GX_SET_ZERO(ray_tracing_pipeline_features);
    ray_tracing_pipeline_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
    ray_tracing_pipeline_features.pNext = &ray_query_features;

    dynamic_rendering_features.pNext = rtx_supported ? static_cast<void*>(&ray_tracing_pipeline_features) : nullptr;
    vulkan_13_features.pNext = &dynamic_rendering_features;
    vulkan_12_features.pNext = &vulkan_13_features;
    info.pNext = &vulkan_12_features;

    vkGetPhysicalDeviceFeatures2(vulkan_data, &info);
    features = info.features;

    const bool dynamic_supported = vulkan_13_features.dynamicRendering == VK_TRUE || dynamic_rendering_features.dynamicRendering == VK_TRUE;
    if (!dynamic_supported) {
        GX_LOG_F("Dynamic rendering feature is required but not supported by the selected physical device.");
    }

    const auto& v12 = vulkan_12_features;
    const bool descriptor_indexing_supported =
        v12.descriptorIndexing == VK_TRUE &&
        v12.runtimeDescriptorArray == VK_TRUE &&
        v12.descriptorBindingPartiallyBound == VK_TRUE &&
        v12.descriptorBindingSampledImageUpdateAfterBind == VK_TRUE &&
        v12.descriptorBindingVariableDescriptorCount == VK_TRUE &&
        v12.shaderSampledImageArrayNonUniformIndexing == VK_TRUE;
    if (!descriptor_indexing_supported) {
        GX_LOG_F("Bindless descriptor indexing requirements are not fully supported by the selected physical device.");
    }
}

void gearoenix::vulkan::device::Physical::initialize_properties()
{
    vkGetPhysicalDeviceProperties(vulkan_data, &properties);
    GX_LOG_D("Physical device name is: " << properties.deviceName);
    vkGetPhysicalDeviceMemoryProperties(vulkan_data, &memory_properties);

    std::uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(vulkan_data, &count, nullptr);
    GX_CHECK_NOT_EQUAL_D(0, count);
    queue_family_properties.resize(count);
    vkGetPhysicalDeviceQueueFamilyProperties(vulkan_data, &count, queue_family_properties.data());

    VkPhysicalDeviceProperties2 info;
    GX_SET_ZERO(info);
    info.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

    GX_SET_ZERO(ray_tracing_pipeline_properties);
    ray_tracing_pipeline_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;

    GX_SET_ZERO(descriptor_indexing_properties);
    descriptor_indexing_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES;

    descriptor_indexing_properties.pNext = rtx_supported ? static_cast<void*>(&ray_tracing_pipeline_properties) : nullptr;
    info.pNext = &descriptor_indexing_properties;
    vkGetPhysicalDeviceProperties2(vulkan_data, &info);
}

gearoenix::vulkan::device::Physical::Physical()
    : Singleton(this)
    , features { }
    , ray_query_features { }
    , ray_tracing_pipeline_features { }
    , shader_clock_features { }
    , vulkan_12_features { }
    , vulkan_13_features { }
    , dynamic_rendering_features { }
    , descriptor_indexing_properties { }
    , properties { }
    , memory_properties { }
    , ray_tracing_pipeline_properties { }
{
    const auto gpus = get_available_devices(Instance::get().get_vulkan_data());
    int best_device_index = -1;
    int best_device_point = -1;

    for (std::uint32_t i = 0; i < gpus.size(); ++i) {
        vkGetPhysicalDeviceProperties(gpus[i], &properties);
        GX_LOG_D("GPU device " << i << ": " << properties.deviceName);
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
        GX_LOG_F("Physical device minimum requirement is not satisfied.");
    }
    vulkan_data = gpus[best_device_index];
    GX_LOG_D("Physical device point is: " << is_good(vulkan_data));

    initialize_extensions();
    initialize_features();
    initialize_properties();

    std::uint32_t count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(vulkan_data, Surface::get().get_vulkan_data(), &count, nullptr);
    surface_formats.resize(static_cast<std::uint64_t>(count));
    vkGetPhysicalDeviceSurfaceFormatsKHR(vulkan_data, Surface::get().get_vulkan_data(), &count, surface_formats.data());

    VkFormatProperties format_props;
    for (auto format : acceptable_depth_formats) {
        vkGetPhysicalDeviceFormatProperties(vulkan_data, format, &format_props);
        if GX_FLAG_CHECK (format_props.optimalTilingFeatures, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            supported_depth_format = format;
            break;
        }
    }
    if (VK_FORMAT_UNDEFINED == supported_depth_format) {
        GX_LOG_F("Error required depth format not found.");
    }

    auto& limits = properties.limits;
    max_memory_alignment =
        std::max(
            std::max(
                std::max(
                    static_cast<std::uint32_t>(limits.minMemoryMapAlignment),
                    static_cast<std::uint32_t>(limits.minStorageBufferOffsetAlignment)
                ),
                std::max(
                    static_cast<std::uint32_t>(limits.minTexelBufferOffsetAlignment),
                    static_cast<std::uint32_t>(limits.minUniformBufferOffsetAlignment)
                )
            ),
            std::max(
                std::max(
                    static_cast<std::uint32_t>(limits.optimalBufferCopyOffsetAlignment),
                    static_cast<std::uint32_t>(limits.optimalBufferCopyRowPitchAlignment)
                ),
            static_cast<std::uint32_t>(limits.bufferImageGranularity)
            )
        );
}

gearoenix::vulkan::device::Physical::~Physical() = default;

std::uint32_t gearoenix::vulkan::device::Physical::get_memory_type_index(
    std::uint32_t type_bits, const std::uint32_t mem_properties) const
{
    for (uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i) {
        if ((type_bits & 1u) == 1u) {
            if (((memory_properties.memoryTypes[i].propertyFlags) & mem_properties) == mem_properties) {
                return i;
            }
        }
        type_bits >>= 1u;
    }
    GX_LOG_F("Could not find the requested memory type.");
}

std::uint64_t gearoenix::vulkan::device::Physical::align_size(const std::uint64_t size) const
{
    return math::Numeric::align(size, static_cast<std::uint64_t>(max_memory_alignment));
}

VkSurfaceCapabilitiesKHR gearoenix::vulkan::device::Physical::get_surface_capabilities() const
{
    VkSurfaceCapabilitiesKHR info;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vulkan_data, Surface::get().get_vulkan_data(), &info);
    return info;
}

std::vector<VkPhysicalDevice> gearoenix::vulkan::device::Physical::get_available_devices(VkInstance instance)
{
    std::uint32_t gpu_count = 0;
    GX_VK_CHK(vkEnumeratePhysicalDevices(instance, &gpu_count, nullptr));
    std::vector<VkPhysicalDevice> gpus(static_cast<std::uint64_t>(gpu_count));
    GX_LOG_D(gpu_count << " GPU(s) are available.");
    GX_VK_CHK(vkEnumeratePhysicalDevices(instance, &gpu_count, gpus.data()));
    return gpus;
}

#endif
