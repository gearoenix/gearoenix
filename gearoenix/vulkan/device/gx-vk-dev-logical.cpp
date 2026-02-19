#include "gx-vk-dev-logical.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../gx-vk-check.hpp"
#include "../gx-vk-instance.hpp"
#include "gx-vk-dev-physical.hpp"

#include <array>
#include <limits>

#include <vulkan/vulkan_beta.h>

gearoenix::vulkan::device::Logical::Logical()
    : Singleton(this)
{
    auto& physical = Physical::get();
    const auto& supported_features = physical.get_features();
    const auto& supported_v12_features = physical.get_vulkan_12_features();
    const auto& supported_v13_features = physical.get_vulkan_13_features();
    const auto& supported_dynamic_features = physical.get_dynamic_rendering_features();
    const auto& supported_rt_pipeline_features = physical.get_ray_tracing_pipeline_features();
    const auto& supported_ray_query_features = physical.get_ray_query_features();
    const auto& supported_shader_clock_features = physical.get_shader_clock_features();

    VkPhysicalDeviceFeatures enabled_features;
    GX_SET_ZERO(enabled_features);
    GX_ASSERT(supported_features.samplerAnisotropy == VK_TRUE);
    enabled_features.samplerAnisotropy = VK_TRUE;
    GX_ASSERT(supported_features.shaderInt64 == VK_TRUE);
    enabled_features.shaderInt64 = VK_TRUE;

    VkPhysicalDeviceVulkan12Features enabled_v12_features;
    GX_SET_ZERO(enabled_v12_features);
    enabled_v12_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    GX_ASSERT(supported_v12_features.descriptorIndexing == VK_TRUE);
    enabled_v12_features.descriptorIndexing = VK_TRUE;
    GX_ASSERT(supported_v12_features.runtimeDescriptorArray == VK_TRUE);
    enabled_v12_features.runtimeDescriptorArray = VK_TRUE;
    GX_ASSERT(supported_v12_features.descriptorBindingPartiallyBound == VK_TRUE);
    enabled_v12_features.descriptorBindingPartiallyBound = VK_TRUE;
    GX_ASSERT(supported_v12_features.descriptorBindingSampledImageUpdateAfterBind == VK_TRUE);
    enabled_v12_features.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;
    GX_ASSERT(supported_v12_features.descriptorBindingVariableDescriptorCount == VK_TRUE);
    enabled_v12_features.descriptorBindingVariableDescriptorCount = VK_TRUE;
    GX_ASSERT(supported_v12_features.shaderSampledImageArrayNonUniformIndexing == VK_TRUE);
    enabled_v12_features.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
    GX_ASSERT(supported_v12_features.descriptorBindingUpdateUnusedWhilePending == VK_TRUE);
    enabled_v12_features.descriptorBindingUpdateUnusedWhilePending = VK_TRUE;
    GX_ASSERT(supported_v12_features.bufferDeviceAddress == VK_TRUE);
    enabled_v12_features.bufferDeviceAddress = VK_TRUE;
    GX_ASSERT(supported_v12_features.scalarBlockLayout == VK_TRUE);
    enabled_v12_features.scalarBlockLayout = VK_TRUE;

    VkPhysicalDeviceVulkan13Features enabled_v13_features;
    GX_SET_ZERO(enabled_v13_features);
    enabled_v13_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    // Dynamic rendering must be supported by the selected physical device.
    GX_ASSERT(supported_v13_features.dynamicRendering == VK_TRUE || supported_dynamic_features.dynamicRendering == VK_TRUE);
    enabled_v13_features.dynamicRendering = VK_TRUE;
    GX_ASSERT(supported_v13_features.synchronization2 == VK_TRUE);
    enabled_v13_features.synchronization2 = VK_TRUE;
    // shaderDemoteToHelperInvocation is required but not supported by the selected physical device.
    GX_ASSERT(supported_v13_features.shaderDemoteToHelperInvocation == VK_TRUE);
    enabled_v13_features.shaderDemoteToHelperInvocation = VK_TRUE;

    VkPhysicalDeviceShaderClockFeaturesKHR enabled_shader_clock_features;
    GX_SET_ZERO(enabled_shader_clock_features);
    enabled_shader_clock_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CLOCK_FEATURES_KHR;

    VkPhysicalDeviceRayQueryFeaturesKHR enabled_ray_query_features;
    GX_SET_ZERO(enabled_ray_query_features);
    enabled_ray_query_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR;

    VkPhysicalDeviceAccelerationStructureFeaturesKHR enabled_acceleration_structure_features;
    GX_SET_ZERO(enabled_acceleration_structure_features);
    enabled_acceleration_structure_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;

    VkPhysicalDeviceRayTracingPipelineFeaturesKHR enabled_ray_tracing_pipeline_features;
    GX_SET_ZERO(enabled_ray_tracing_pipeline_features);
    enabled_ray_tracing_pipeline_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;

    if (physical.get_rtx_supported()) {
        GX_ASSERT(supported_shader_clock_features.shaderSubgroupClock == VK_TRUE);
        GX_ASSERT(supported_ray_query_features.rayQuery == VK_TRUE);
        GX_ASSERT(supported_rt_pipeline_features.rayTracingPipeline == VK_TRUE);

        enabled_shader_clock_features.shaderSubgroupClock = VK_TRUE;

        enabled_ray_query_features.rayQuery = VK_TRUE;
        enabled_ray_query_features.pNext = &enabled_shader_clock_features;

        enabled_ray_tracing_pipeline_features.rayTracingPipeline = VK_TRUE;
        enabled_ray_tracing_pipeline_features.rayTraversalPrimitiveCulling = supported_rt_pipeline_features.rayTraversalPrimitiveCulling;
        enabled_ray_tracing_pipeline_features.pNext = &enabled_ray_query_features;

        enabled_acceleration_structure_features.accelerationStructure = VK_TRUE;
        enabled_acceleration_structure_features.pNext = &enabled_ray_tracing_pipeline_features;

        enabled_v13_features.pNext = &enabled_acceleration_structure_features;
    } else {
        enabled_v13_features.pNext = nullptr;
    }
    enabled_v12_features.pNext = &enabled_v13_features;

    VkPhysicalDeviceFeatures2 device_features2;
    GX_SET_ZERO(device_features2);
    device_features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    device_features2.features = enabled_features;
    device_features2.pNext = &enabled_v12_features;

    std::vector<const char*> device_extensions;
    device_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    GX_ASSERT_D(physical.get_supported_extensions().contains(VK_KHR_SWAPCHAIN_MUTABLE_FORMAT_EXTENSION_NAME));
    device_extensions.push_back(VK_KHR_SWAPCHAIN_MUTABLE_FORMAT_EXTENSION_NAME);
    if (physical.get_supported_extensions().contains(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME)) {
        device_extensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
    }
    if (physical.get_rtx_supported()) {
        device_extensions.push_back(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);
        device_extensions.push_back(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
        device_extensions.push_back(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);
        device_extensions.push_back(VK_KHR_RAY_QUERY_EXTENSION_NAME);
        device_extensions.push_back(VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME);
        device_extensions.push_back(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
        device_extensions.push_back(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME);
        device_extensions.push_back(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME);
        device_extensions.push_back(VK_KHR_SHADER_CLOCK_EXTENSION_NAME);
    }

    constexpr std::array queue_priorities { 1.0f }; // Some devices only support one queue!

    std::set<std::uint32_t> queue_index_set;
    queue_index_set.insert(physical.get_graphics_queue_node_index());
    queue_index_set.insert(physical.get_transfer_queue_node_index());
    queue_index_set.insert(physical.get_compute_queue_node_index());
    queue_index_set.insert(physical.get_present_queue_node_index());

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    queue_create_infos.reserve(static_cast<std::uint64_t>(queue_index_set.size()));

    for (const auto q : queue_index_set) {
        if (q == std::numeric_limits<std::uint32_t>::max()) {
            continue;
        }
        queue_create_infos.emplace_back();
        auto& queue_create_info = queue_create_infos.back();
        GX_SET_ZERO(queue_create_info);
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueCount = static_cast<std::uint32_t>(queue_priorities.size());
        queue_create_info.queueFamilyIndex = q;
        queue_create_info.pQueuePriorities = queue_priorities.data();
        GX_LOG_D("queue node index added is " << q);
    }

    VkDeviceCreateInfo device_create_info;
    GX_SET_ZERO(device_create_info);
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.queueCreateInfoCount = static_cast<std::uint32_t>(queue_create_infos.size());
    device_create_info.pQueueCreateInfos = queue_create_infos.data();
    device_create_info.enabledExtensionCount = static_cast<std::uint32_t>(device_extensions.size());
    device_create_info.ppEnabledExtensionNames = device_extensions.data();
    device_create_info.pNext = &device_features2;
    GX_VK_CHK(vkCreateDevice(physical.get_vulkan_data(), &device_create_info, nullptr, &vulkan_data));

    Loader::load(vulkan_data);

#if GX_VULKAN_DEVICE_DEBUG_MODE
    debug_marker_is_available = vkSetDebugUtilsObjectNameEXT != nullptr;
#endif
}

gearoenix::vulkan::device::Logical::~Logical()
{
    if (vulkan_data != nullptr) {
        vkDestroyDevice(vulkan_data, nullptr);
        vulkan_data = nullptr;
    }
}

void gearoenix::vulkan::device::Logical::wait_to_finish() { vkDeviceWaitIdle(vulkan_data); }

#endif
