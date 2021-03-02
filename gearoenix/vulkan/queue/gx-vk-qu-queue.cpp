#include "gx-vk-qu-queue.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../device/gx-vk-dev-logical.hpp"

gearoenix::vulkan::queue::Queue::Queue(const device::Logical& logical_device, VkQueue vulkan_data) noexcept
    : logical_device(logical_device)
    , vulkan_data(vulkan_data)
{
}

gearoenix::vulkan::queue::Queue::Queue(
    const device::Logical& logical_device,
    const std::uint32_t node_index) noexcept
    : logical_device(logical_device)
{
    vkGetDeviceQueue(logical_device.get_vulkan_data(), node_index, 0, &vulkan_data);
}

gearoenix::vulkan::queue::Queue::~Queue() noexcept = default;

#endif