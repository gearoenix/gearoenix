#include "gx-vk-cmd-pool.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"

gearoenix::vulkan::command::Pool::Pool()
    : vulkan_data(
          device::Logical::get().get_device(),
          vk::CommandPoolCreateInfo(
              vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
              device::Physical::get().get_graphics_queue_node_index()))
{
}

gearoenix::vulkan::command::Pool::~Pool() = default;

#endif
