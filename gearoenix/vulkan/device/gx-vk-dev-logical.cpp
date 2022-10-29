#include "gx-vk-dev-logical.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../gx-vk-check.hpp"
#include "../gx-vk-instance.hpp"
#include "gx-vk-dev-physical.hpp"
#include <array>

gearoenix::vulkan::device::Logical::Logical(const Physical& p) noexcept
    : physical_device(p)
{
    VkPhysicalDeviceFeatures device_features;
    GX_SET_ZERO(device_features);
    device_features.samplerAnisotropy = VK_TRUE;
    device_features.shaderInt64 = VK_TRUE;

    VkPhysicalDeviceDescriptorIndexingFeatures descriptor_indexing_features;
    GX_SET_ZERO(descriptor_indexing_features);
    descriptor_indexing_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
    descriptor_indexing_features.runtimeDescriptorArray = VK_TRUE;
    descriptor_indexing_features.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;

    VkPhysicalDeviceShaderClockFeaturesKHR shader_clock_features;
    GX_SET_ZERO(shader_clock_features);
    shader_clock_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CLOCK_FEATURES_KHR;
    shader_clock_features.shaderSubgroupClock = VK_TRUE;
    shader_clock_features.pNext = &descriptor_indexing_features;

    VkPhysicalDeviceScalarBlockLayoutFeatures layout_features;
    GX_SET_ZERO(layout_features);
    layout_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES;
    layout_features.scalarBlockLayout = VK_TRUE;
    layout_features.pNext = &shader_clock_features;

    VkPhysicalDeviceBufferDeviceAddressFeatures device_features_bda;
    GX_SET_ZERO(device_features_bda);
    device_features_bda.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
    device_features_bda.bufferDeviceAddress = VK_TRUE;
    device_features_bda.pNext = &layout_features;

    VkPhysicalDeviceAccelerationStructureFeaturesKHR device_features_as;
    GX_SET_ZERO(device_features_as);
    device_features_as.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
    device_features_as.accelerationStructure = VK_TRUE;
    device_features_as.pNext = &device_features_bda;

    VkPhysicalDeviceRayTracingPipelineFeaturesKHR device_features_rtp;
    GX_SET_ZERO(device_features_rtp);
    device_features_rtp.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
    device_features_rtp.rayTracingPipeline = VK_TRUE;
    device_features_rtp.rayTraversalPrimitiveCulling = VK_TRUE;
    device_features_rtp.pNext = &device_features_as;

    VkPhysicalDeviceFeatures2 device_features2;
    GX_SET_ZERO(device_features2);
    device_features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    device_features2.features = device_features;
    device_features2.pNext = &device_features_rtp;

    std::vector<const char*> device_extensions;
    device_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    const auto& available_extensions = p.get_supported_extensions();
#ifdef GX_VULKAN_DEVICE_DEBUG_MODE
    if (available_extensions.contains(VK_EXT_DEBUG_MARKER_EXTENSION_NAME)) {
        device_extensions.push_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
        debug_marker_is_available = true;
    }
#endif
    if (p.get_rtx_supported()) {
        device_extensions.push_back(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
        device_extensions.push_back(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME);
        device_extensions.push_back(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME);
        device_extensions.push_back(VK_KHR_MAINTENANCE3_EXTENSION_NAME);
        device_extensions.push_back(VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME);
        device_extensions.push_back(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
        device_extensions.push_back(VK_KHR_SHADER_CLOCK_EXTENSION_NAME);
        device_extensions.push_back(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);
        device_extensions.push_back(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
        device_extensions.push_back(VK_KHR_RAY_QUERY_EXTENSION_NAME);
        device_extensions.push_back(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);
    }
    const std::array<float, 8> queue_priorities { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
    std::set<std::uint32_t> queue_index_set;
    queue_index_set.insert(physical_device.get_graphics_queue_node_index());
    queue_index_set.insert(physical_device.get_transfer_queue_node_index());
    queue_index_set.insert(physical_device.get_compute_queue_node_index());
    queue_index_set.insert(physical_device.get_present_queue_node_index());
    std::vector<VkDeviceQueueCreateInfo> queue_create_infos(static_cast<std::size_t>(queue_index_set.size()));
    std::size_t queue_create_infos_index = 0;
    for (auto q : queue_index_set) {
        if (q == UINT32_MAX)
            continue;
        auto& queue_create_info = queue_create_infos[queue_create_infos_index++];
        GX_SET_ZERO(queue_create_info);
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueCount = queue_priorities.size();
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
    if (p.get_rtx_supported()) {
        device_create_info.pNext = &device_features2;
    } else {
        device_create_info.pEnabledFeatures = &device_features;
    }
    GX_VK_CHK(vkCreateDevice(physical_device.get_vulkan_data(), &device_create_info, nullptr, &vulkan_data));
    Loader::load(vulkan_data);
}

gearoenix::vulkan::device::Logical::~Logical() noexcept
{
    if (vulkan_data != nullptr) {
        vkDestroyDevice(vulkan_data, nullptr);
        vulkan_data = nullptr;
    }
}

void gearoenix::vulkan::device::Logical::wait_to_finish() noexcept
{
    vkDeviceWaitIdle(vulkan_data);
}

#endif
