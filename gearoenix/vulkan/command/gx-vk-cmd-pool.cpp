#include "gx-vk-cmd-pool.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../gx-vk-check.hpp"

gearoenix::vulkan::command::Pool::Pool(Pool&& o) noexcept
    : logical_device(o.logical_device)
    , vulkan_data(o.vulkan_data)
{
    o.vulkan_data = nullptr;
}

gearoenix::vulkan::command::Pool::Pool(const device::Logical& ld) noexcept
    : logical_device(ld)
{
    VkCommandPoolCreateInfo info;
    GX_SET_ZERO(info)
    info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    info.queueFamilyIndex = logical_device.get_physical_device().get_graphics_queue_node_index();
    info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    GX_VK_CHK_L(vkCreateCommandPool(logical_device.get_vulkan_data(), &info, nullptr, &vulkan_data))
}

gearoenix::vulkan::command::Pool::~Pool() noexcept
{
    if (nullptr != vulkan_data) {
        Loader::vkDestroyCommandPool(logical_device.get_vulkan_data(), vulkan_data, nullptr);
        vulkan_data = nullptr;
    }
}

#endif
