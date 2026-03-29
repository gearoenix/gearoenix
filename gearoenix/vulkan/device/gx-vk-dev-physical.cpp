#include "gx-vk-dev-physical.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../math/gx-math-numeric.hpp"
#include "../../platform/gx-plt-log.hpp"
#include "../gx-vk-instance.hpp"
#include "../gx-vk-surface.hpp"

#include <algorithm>

int gearoenix::vulkan::device::Physical::is_good(const vk::raii::PhysicalDevice& gpu)
{
    graphics_queue_node_index = std::numeric_limits<std::uint32_t>::max();
    transfer_queue_node_index = std::numeric_limits<std::uint32_t>::max();
    compute_queue_node_index = std::numeric_limits<std::uint32_t>::max();
    present_queue_node_index = std::numeric_limits<std::uint32_t>::max();

    const auto candidate_properties = gpu.getProperties();
    if (vk::apiVersionMajor(candidate_properties.apiVersion) < 1 || (vk::apiVersionMajor(candidate_properties.apiVersion) == 1 && vk::apiVersionMinor(candidate_properties.apiVersion) < 3)) {
        GX_LOG_D("Skipping GPU " << candidate_properties.deviceName.data() << " because it does not meet the Vulkan 1.3 requirement.");
        return -1;
    }

    const auto features_chain = gpu.getFeatures2<
        vk::PhysicalDeviceFeatures2,
        vk::PhysicalDeviceVulkan12Features,
        vk::PhysicalDeviceVulkan13Features,
        vk::PhysicalDeviceDynamicRenderingFeatures>();

    const auto& v12_features = features_chain.get<vk::PhysicalDeviceVulkan12Features>();
    const auto& v13_features = features_chain.get<vk::PhysicalDeviceVulkan13Features>();
    const auto& dynamic_features = features_chain.get<vk::PhysicalDeviceDynamicRenderingFeatures>();

    if (v13_features.dynamicRendering == vk::False && dynamic_features.dynamicRendering == vk::False) {
        GX_LOG_D("Skipping GPU " << candidate_properties.deviceName.data() << " because dynamic rendering is not supported.");
        return -1;
    }

    bool descriptor_indexing_supported = v12_features.descriptorIndexing == vk::True;
    descriptor_indexing_supported = descriptor_indexing_supported && v12_features.runtimeDescriptorArray == vk::True;
    descriptor_indexing_supported = descriptor_indexing_supported && v12_features.descriptorBindingPartiallyBound == vk::True;
    descriptor_indexing_supported = descriptor_indexing_supported && v12_features.descriptorBindingSampledImageUpdateAfterBind == vk::True;
    descriptor_indexing_supported = descriptor_indexing_supported && v12_features.descriptorBindingVariableDescriptorCount == vk::True;
    descriptor_indexing_supported = descriptor_indexing_supported && v12_features.shaderSampledImageArrayNonUniformIndexing == vk::True;
    if (!descriptor_indexing_supported) {
        GX_LOG_D("Skipping GPU " << candidate_properties.deviceName.data() << " because required descriptor indexing features are missing.");
        return -1;
    }

    const auto queue_props = gpu.getQueueFamilyProperties();
    const auto queue_count = static_cast<std::uint32_t>(queue_props.size());
    if (queue_count == 0) {
        return -1;
    }

    std::vector<vk::Bool32> supports_present(queue_count);
    for (uint32_t i = 0; i < queue_count; ++i) {
        supports_present[i] = gpu.getSurfaceSupportKHR(i, Surface::get().get_vulkan_data());
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

        if ((qf & vk::QueueFlagBits::eGraphics) && (qf & vk::QueueFlagBits::eTransfer) && (qf & vk::QueueFlagBits::eCompute) && sp == vk::True) {
            return commit_and_return(i, i, i, i, 100);
        }
        if (qf & vk::QueueFlagBits::eGraphics) {
            graphics_candidate = i;
        }
        if (qf & vk::QueueFlagBits::eTransfer) {
            transfer_candidate = i;
        }
        if (qf & vk::QueueFlagBits::eCompute) {
            compute_candidate = i;
        }
        if (sp == vk::True) {
            present_candidate = i;
        }
    }

    for (std::uint32_t i = 0; i < queue_count; ++i) {
        const auto qf = queue_props[i].queueFlags;
        const auto sp = supports_present[i];

        if ((qf & vk::QueueFlagBits::eGraphics) && (qf & vk::QueueFlagBits::eTransfer) && sp == vk::True) {
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

        if ((qf & vk::QueueFlagBits::eGraphics) && sp == vk::True) {
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
    const auto extensions = vulkan_data.enumerateDeviceExtensionProperties();
    for (const auto& [extensionName, specVersion] : extensions) {
        supported_extensions.emplace(extensionName.data());
    }

    rtx_supported = supported_extensions.contains(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
    rtx_supported = rtx_supported && supported_extensions.contains(VK_KHR_RAY_QUERY_EXTENSION_NAME);
    rtx_supported = rtx_supported && supported_extensions.contains(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);
    rtx_supported = rtx_supported && supported_extensions.contains(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);

#if GX_DEBUG_MODE
    GX_LOG_D("Supported extensions are:");
    for (auto& s : supported_extensions) {
        GX_LOG_D("    " << s);
    }
#endif
}

void gearoenix::vulkan::device::Physical::initialize_features()
{
    features = vulkan_data.getFeatures();

    const auto features_chain = vulkan_data.getFeatures2<
        vk::PhysicalDeviceFeatures2,
        vk::PhysicalDeviceVulkan12Features,
        vk::PhysicalDeviceVulkan13Features,
        vk::PhysicalDeviceDynamicRenderingFeatures,
        vk::PhysicalDeviceRayTracingPipelineFeaturesKHR,
        vk::PhysicalDeviceRayQueryFeaturesKHR,
        vk::PhysicalDeviceShaderClockFeaturesKHR>();
    features = features_chain.get<vk::PhysicalDeviceFeatures2>().features;
    vulkan_12_features = features_chain.get<vk::PhysicalDeviceVulkan12Features>();
    vulkan_13_features = features_chain.get<vk::PhysicalDeviceVulkan13Features>();
    dynamic_rendering_features = features_chain.get<vk::PhysicalDeviceDynamicRenderingFeatures>();
    ray_tracing_pipeline_features = features_chain.get<vk::PhysicalDeviceRayTracingPipelineFeaturesKHR>();
    ray_query_features = features_chain.get<vk::PhysicalDeviceRayQueryFeaturesKHR>();
    shader_clock_features = features_chain.get<vk::PhysicalDeviceShaderClockFeaturesKHR>();

    if (vulkan_13_features.dynamicRendering == vk::False && dynamic_rendering_features.dynamicRendering == vk::False) {
        GX_LOG_F("Dynamic rendering feature is required but not supported by the selected physical device.");
    }

    bool descriptor_indexing_supported = vulkan_12_features.descriptorIndexing == vk::True;
    descriptor_indexing_supported = descriptor_indexing_supported && vulkan_12_features.runtimeDescriptorArray == vk::True;
    descriptor_indexing_supported = descriptor_indexing_supported && vulkan_12_features.descriptorBindingPartiallyBound == vk::True;
    descriptor_indexing_supported = descriptor_indexing_supported && vulkan_12_features.descriptorBindingSampledImageUpdateAfterBind == vk::True;
    descriptor_indexing_supported = descriptor_indexing_supported && vulkan_12_features.descriptorBindingVariableDescriptorCount == vk::True;
    descriptor_indexing_supported = descriptor_indexing_supported && vulkan_12_features.shaderSampledImageArrayNonUniformIndexing == vk::True;
    if (!descriptor_indexing_supported) {
        GX_LOG_F("Bindless descriptor indexing requirements are not fully supported by the selected physical device.");
    }
}

void gearoenix::vulkan::device::Physical::initialize_properties()
{
    properties = vulkan_data.getProperties();
    GX_LOG_D("Physical device name is: " << properties.deviceName.data());
    memory_properties = vulkan_data.getMemoryProperties();

    queue_family_properties = vulkan_data.getQueueFamilyProperties();
    GX_CHECK_NOT_EQUAL_D(static_cast<std::size_t>(0), queue_family_properties.size());

    auto chain = vulkan_data.getProperties2<
        vk::PhysicalDeviceProperties2,
        vk::PhysicalDeviceDescriptorIndexingProperties,
        vk::PhysicalDeviceRayTracingPipelinePropertiesKHR>();
    descriptor_indexing_properties = chain.get<vk::PhysicalDeviceDescriptorIndexingProperties>();
    ray_tracing_pipeline_properties = chain.get<vk::PhysicalDeviceRayTracingPipelinePropertiesKHR>();
}

gearoenix::vulkan::device::Physical::Physical()
    : Singleton(this)
    , vulkan_data(nullptr)
{
    const auto& instance = Instance::get();
    const auto gpus = instance.get_instance().enumeratePhysicalDevices();
    int best_device_index = -1;
    int best_device_point = -1;

    for (std::uint32_t i = 0; i < gpus.size(); ++i) {
        const auto props = gpus[i].getProperties();
        GX_LOG_D("GPU device " << i << ": " << props.deviceName.data());
        if (vk::PhysicalDeviceType::eDiscreteGpu != props.deviceType) {
            continue;
        }
        if (const auto device_point = is_good(gpus[i]); device_point > best_device_point) {
            best_device_index = static_cast<int>(i);
            best_device_point = device_point;
        }
    }

    if (-1 == best_device_index) {
        for (std::uint32_t i = 0; i < gpus.size(); ++i) {
            if (const auto device_point = is_good(gpus[i]); device_point > best_device_point) {
                best_device_index = static_cast<int>(i);
                best_device_point = device_point;
            }
        }
    }

    if (best_device_index == -1 || best_device_point == -1) {
        GX_LOG_F("Physical device minimum requirement is not satisfied.");
    }

    // Re-enumerate to get a fresh handle for the selected device
    auto selected_gpus = instance.get_instance().enumeratePhysicalDevices();
    vulkan_data = std::move(selected_gpus[best_device_index]);

    GX_LOG_D("Physical device point is: " << is_good(vulkan_data));

    initialize_extensions();
    initialize_features();
    initialize_properties();

    const auto& surface = Surface::get();
    surface_formats = vulkan_data.getSurfaceFormatsKHR(surface.get_vulkan_data());

    for (auto format : acceptable_depth_formats) {
        const auto format_props = vulkan_data.getFormatProperties(format);
        if (format_props.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment) {
            supported_depth_format = format;
            break;
        }
    }
    if (vk::Format::eUndefined == supported_depth_format) {
        GX_LOG_F("Error required depth format not found.");
    }
}

gearoenix::vulkan::device::Physical::~Physical() = default;

std::optional<std::uint32_t> gearoenix::vulkan::device::Physical::get_memory_type_index(std::uint32_t type_bits, const vk::MemoryPropertyFlags mem_properties) const
{
    for (uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i) {
        if (type_bits & 1u) {
            if ((memory_properties.memoryTypes[i].propertyFlags & mem_properties) == mem_properties) {
                return i;
            }
        }
        type_bits >>= 1u;
    }
    return std::nullopt;
}

vk::SurfaceCapabilitiesKHR gearoenix::vulkan::device::Physical::get_surface_capabilities() const
{
    return vulkan_data.getSurfaceCapabilitiesKHR(Surface::get().get_vulkan_data());
}

#endif
