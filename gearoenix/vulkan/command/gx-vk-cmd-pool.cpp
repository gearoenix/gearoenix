#include "gx-vk-cmd-pool.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../gx-vk-check.hpp"

gearoenix::vulkan::command::Pool::Pool()
{
    VkCommandPoolCreateInfo info;
    GX_SET_ZERO(info);
    info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    info.queueFamilyIndex = device::Physical::get().get_graphics_queue_node_index();
    info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    GX_VK_CHK(vkCreateCommandPool(device::Logical::get().get_vulkan_data(), &info, nullptr, &vulkan_data));
}

gearoenix::vulkan::command::Pool::~Pool()
{
    vkDestroyCommandPool(device::Logical::get().get_vulkan_data(), vulkan_data, nullptr);
}

#endif
