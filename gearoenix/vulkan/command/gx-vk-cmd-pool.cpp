#include "gx-vk-cmd-pool.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../gx-vk-check.hpp"

gearoenix::vulkan::command::Pool::Pool(std::shared_ptr<device::Logical> ld) noexcept
    : logical_device(std::move(ld))
{
    VkCommandPoolCreateInfo vk_cmd_pool_info;
    GX_SET_ZERO(vk_cmd_pool_info)
    vk_cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    vk_cmd_pool_info.queueFamilyIndex = logical_device->get_physical_device()->get_graphics_queue_node_index();
    vk_cmd_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    GX_VK_CHK_L(vkCreateCommandPool(logical_device->get_vulkan_data(), &vk_cmd_pool_info, nullptr, &vulkan_data))
}

gearoenix::vulkan::command::Pool::~Pool() noexcept
{
    Loader::vkDestroyCommandPool(logical_device->get_vulkan_data(), vulkan_data, nullptr);
}
#endif
