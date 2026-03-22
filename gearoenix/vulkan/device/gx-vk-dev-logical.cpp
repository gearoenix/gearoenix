#include "gx-vk-dev-logical.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../../platform/gx-plt-log.hpp"
#include "gx-vk-dev-physical.hpp"

#include <array>
#include <limits>
#include <set>

gearoenix::vulkan::device::Logical::Logical()
    : Singleton(this)
    , vulkan_device(nullptr)
{
    auto& physical = Physical::get();
    const auto& supported_features = physical.get_features();
    const auto& supported_v12_features = physical.get_vulkan_12_features();
    const auto& supported_v13_features = physical.get_vulkan_13_features();
    const auto& supported_dynamic_features = physical.get_dynamic_rendering_features();
    const auto& supported_rt_pipeline_features = physical.get_ray_tracing_pipeline_features();
    const auto& supported_ray_query_features = physical.get_ray_query_features();
    const auto& supported_shader_clock_features = physical.get_shader_clock_features();

    vk::PhysicalDeviceFeatures enabled_features;
    GX_ASSERT(supported_features.samplerAnisotropy == vk::True);
    enabled_features.samplerAnisotropy = vk::True;
    GX_ASSERT(supported_features.shaderInt64 == vk::True);
    enabled_features.shaderInt64 = vk::True;

    vk::PhysicalDeviceVulkan12Features enabled_v12_features;
    GX_ASSERT(supported_v12_features.descriptorIndexing == vk::True);
    enabled_v12_features.descriptorIndexing = vk::True;
    GX_ASSERT(supported_v12_features.runtimeDescriptorArray == vk::True);
    enabled_v12_features.runtimeDescriptorArray = vk::True;
    GX_ASSERT(supported_v12_features.descriptorBindingPartiallyBound == vk::True);
    enabled_v12_features.descriptorBindingPartiallyBound = vk::True;
    GX_ASSERT(supported_v12_features.descriptorBindingSampledImageUpdateAfterBind == vk::True);
    enabled_v12_features.descriptorBindingSampledImageUpdateAfterBind = vk::True;
    GX_ASSERT(supported_v12_features.descriptorBindingVariableDescriptorCount == vk::True);
    enabled_v12_features.descriptorBindingVariableDescriptorCount = vk::True;
    GX_ASSERT(supported_v12_features.shaderSampledImageArrayNonUniformIndexing == vk::True);
    enabled_v12_features.shaderSampledImageArrayNonUniformIndexing = vk::True;
    GX_ASSERT(supported_v12_features.descriptorBindingUpdateUnusedWhilePending == vk::True);
    enabled_v12_features.descriptorBindingUpdateUnusedWhilePending = vk::True;
    GX_ASSERT(supported_v12_features.bufferDeviceAddress == vk::True);
    enabled_v12_features.bufferDeviceAddress = vk::True;
    GX_ASSERT(supported_v12_features.scalarBlockLayout == vk::True);
    enabled_v12_features.scalarBlockLayout = vk::True;

    vk::PhysicalDeviceVulkan13Features enabled_v13_features;
    GX_ASSERT(supported_v13_features.dynamicRendering == vk::True || supported_dynamic_features.dynamicRendering == vk::True);
    enabled_v13_features.dynamicRendering = vk::True;
    GX_ASSERT(supported_v13_features.synchronization2 == vk::True);
    enabled_v13_features.synchronization2 = vk::True;
    GX_ASSERT(supported_v13_features.shaderDemoteToHelperInvocation == vk::True);
    enabled_v13_features.shaderDemoteToHelperInvocation = vk::True;

    vk::PhysicalDeviceShaderClockFeaturesKHR enabled_shader_clock_features;
    vk::PhysicalDeviceRayQueryFeaturesKHR enabled_ray_query_features;
    vk::PhysicalDeviceAccelerationStructureFeaturesKHR enabled_acceleration_structure_features;
    vk::PhysicalDeviceRayTracingPipelineFeaturesKHR enabled_ray_tracing_pipeline_features;

    if (physical.get_rtx_supported()) {
        GX_ASSERT(supported_shader_clock_features.shaderSubgroupClock == vk::True);
        GX_ASSERT(supported_ray_query_features.rayQuery == vk::True);
        GX_ASSERT(supported_rt_pipeline_features.rayTracingPipeline == vk::True);

        enabled_shader_clock_features.shaderSubgroupClock = vk::True;

        enabled_ray_query_features.rayQuery = vk::True;
        enabled_ray_query_features.pNext = &enabled_shader_clock_features;

        enabled_ray_tracing_pipeline_features.rayTracingPipeline = vk::True;
        enabled_ray_tracing_pipeline_features.rayTraversalPrimitiveCulling = supported_rt_pipeline_features.rayTraversalPrimitiveCulling;
        enabled_ray_tracing_pipeline_features.pNext = &enabled_ray_query_features;

        enabled_acceleration_structure_features.accelerationStructure = vk::True;
        enabled_acceleration_structure_features.pNext = &enabled_ray_tracing_pipeline_features;

        enabled_v13_features.pNext = &enabled_acceleration_structure_features;
    } else {
        enabled_v13_features.pNext = nullptr;
    }
    enabled_v12_features.pNext = &enabled_v13_features;

    vk::PhysicalDeviceFeatures2 device_features2;
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

    std::set<std::uint32_t> queue_index_set;
    queue_index_set.insert(physical.get_graphics_queue_node_index());
    queue_index_set.insert(physical.get_transfer_queue_node_index());
    queue_index_set.insert(physical.get_compute_queue_node_index());
    queue_index_set.insert(physical.get_present_queue_node_index());

    std::vector<vk::DeviceQueueCreateInfo> queue_create_infos;
    queue_create_infos.reserve(queue_index_set.size());

    for (const auto q : queue_index_set) {
        constexpr std::array queue_priorities { 1.0f };

        if (q == std::numeric_limits<std::uint32_t>::max()) {
            continue;
        }
        queue_create_infos.emplace_back(vk::DeviceQueueCreateFlags { }, q, queue_priorities);
        GX_LOG_D("queue node index added is " << q);
    }

    vk::DeviceCreateInfo device_create_info(
        { },
        queue_create_infos,
        { },
        device_extensions);
    device_create_info.setPNext(&device_features2);

    vulkan_device = physical.get_physical_device().createDevice(device_create_info);

    Loader::load(*vulkan_device);

#if GX_VULKAN_DEVICE_DEBUG_MODE
    debug_marker_is_available = VULKAN_HPP_DEFAULT_DISPATCHER.vkSetDebugUtilsObjectNameEXT != nullptr;
#endif
}

gearoenix::vulkan::device::Logical::~Logical() = default;

void gearoenix::vulkan::device::Logical::wait_to_finish()
{
    vulkan_device.waitIdle();
}

#endif
