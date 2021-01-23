#include "gx-vk-dev-logical.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../gx-vk-check.hpp"
#include "../gx-vk-instance.hpp"
#include "gx-vk-dev-physical.hpp"

gearoenix::vulkan::device::Logical::Logical(const Physical& p) noexcept
    : physical_device(p)
{
    std::vector<const char*> device_extensions;
    device_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
#ifdef GX_VULKAN_DEVICE_DEBUG_MODE
    if (p.get_supported_extensions().contains(VK_EXT_DEBUG_MARKER_EXTENSION_NAME)) {
        device_extensions.push_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
    }
#endif
    const float queue_priorities[] = { 1.0f };
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
        GX_SET_ZERO(queue_create_info)
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueCount = 1;
        queue_create_info.queueFamilyIndex = q;
        queue_create_info.pQueuePriorities = queue_priorities;
        GX_LOG_D("queue node index added is " << q)
    }
    VkPhysicalDeviceFeatures device_features;
    GX_SET_ZERO(device_features)
    device_features.samplerAnisotropy = VK_TRUE;
    VkDeviceCreateInfo device_create_info;
    GX_SET_ZERO(device_create_info)
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.queueCreateInfoCount = static_cast<std::uint32_t>(queue_create_infos.size());
    device_create_info.pQueueCreateInfos = queue_create_infos.data();
    device_create_info.enabledExtensionCount = static_cast<std::uint32_t>(device_extensions.size());
    device_create_info.ppEnabledExtensionNames = device_extensions.data();
    device_create_info.pEnabledFeatures = &device_features;
    GX_VK_CHK_L(vkCreateDevice(physical_device.get_vulkan_data(), &device_create_info, nullptr, &vulkan_data))
    Loader::vkGetDeviceQueue(vulkan_data, physical_device.get_graphics_queue_node_index(), 0, &graphic_queue);
    Loader::load(vulkan_data);
}

gearoenix::vulkan::device::Logical::~Logical() noexcept
{
    Loader::vkDestroyDevice(vulkan_data, nullptr);
}

void gearoenix::vulkan::device::Logical::wait_to_finish() noexcept
{
    Loader::vkDeviceWaitIdle(vulkan_data);
}
#endif
