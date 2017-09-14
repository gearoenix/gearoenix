#include "vk-cmd-pool.hpp"
#include "../../core/cr-static.hpp"
#include "../device/vk-dev-logical.hpp"
#include "../device/vk-dev-physical.hpp"
#include "../vk-check.hpp"
#include "../vk-instance.hpp"

gearoenix::render::command::Pool::Pool(
    const std::shared_ptr<device::Logical>& logical_device)
    : logical_device(logical_device)
{
    auto p = logical_device->get_physical_device();
    auto l = p->get_instance()->get_linker();
    VkCommandPoolCreateInfo vk_cmd_pool_info;
    setz(vk_cmd_pool_info);
    vk_cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    vk_cmd_pool_info.queueFamilyIndex = p->get_graphics_queue_node_index();
    vk_cmd_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    VKC(l->vkCreateCommandPool(logical_device->get_vulkan_data(),
        &vk_cmd_pool_info, nullptr, &vulkan_data));
}

gearoenix::render::command::Pool::~Pool()
{
    auto p = logical_device->get_physical_device();
    auto l = p->get_instance()->get_linker();
    l->vkDestroyCommandPool(logical_device->get_vulkan_data(), vulkan_data,
        nullptr);
}

const VkCommandPool& gearoenix::render::command::Pool::get_vulkan_data() const
{
    return vulkan_data;
}

const std::shared_ptr<gearoenix::render::device::Logical>& gearoenix::render::command::Pool::get_logical_device() const
{
    return logical_device;
}

std::shared_ptr<gearoenix::render::device::Logical>& gearoenix::render::command::Pool::get_logical_device()
{
    return logical_device;
}
