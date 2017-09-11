#include "../../core/cr-build-configuration.hpp"
#ifdef DEBUG_MODE
#define VK_CMD_BUFF_DEBUG
#endif
#include "../../core/cr-static.hpp"
#include "../device/vk-dev-logical.hpp"
#include "../device/vk-dev-physical.hpp"
#include "../sync/vk-sync-fence.hpp"
#include "../vk-check.hpp"
#include "../vk-instance.hpp"
#include "vk-cmd-buffer.hpp"
#include "vk-cmd-pool.hpp"

gearoenix::render::command::Buffer::Buffer(const std::shared_ptr<Pool>& pool)
    : pool(pool)
{
    auto& l = pool->get_logical_device()
                  ->get_physical_device()
                  ->get_instance()
                  ->get_linker();
    linker = l;
    VkCommandBufferAllocateInfo cmd_buf_allocate_info;
    setz(cmd_buf_allocate_info);
    cmd_buf_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmd_buf_allocate_info.commandPool = pool->get_vulkan_data();
    cmd_buf_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmd_buf_allocate_info.commandBufferCount = 1;
    VKC(l->vkAllocateCommandBuffers(pool->get_logical_device()->get_vulkan_data(),
        &cmd_buf_allocate_info, &vulkan_data));
    VkCommandBufferBeginInfo cmd_buf_info;
    setz(cmd_buf_info);
    cmd_buf_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    VKC(l->vkBeginCommandBuffer(vulkan_data, &cmd_buf_info));
}

gearoenix::render::command::Buffer::~Buffer()
{
    if (not_flushed) {
        flush();
    }
    auto& d = pool->get_logical_device();
    linker->vkFreeCommandBuffers(d->get_vulkan_data(), pool->get_vulkan_data(), 1,
        &vulkan_data);
}

void gearoenix::render::command::Buffer::copy_buffer(
    VkBuffer src, VkBuffer des, const VkBufferCopy& region)
{
    linker->vkCmdCopyBuffer(vulkan_data, src, des, 1, &region);
    not_flushed = true;
}

void gearoenix::render::command::Buffer::flush()
{
    not_flushed = false;
    auto& d = pool->get_logical_device();
    sync::Fence fence(d);
    VKC(linker->vkEndCommandBuffer(vulkan_data));
    VkSubmitInfo submit_info;
    setz(submit_info);
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &vulkan_data;
    VKC(linker->vkQueueSubmit(d->get_graphic_queue(), 1, &submit_info,
        fence.get_vulkan_data()));
    fence.wait();
}

const VkCommandBuffer& gearoenix::render::command::Buffer::get_vulkan_data()
    const
{
    return vulkan_data;
}

void gearoenix::render::command::Buffer::begin_render_pass_with_info(
    const VkRenderPassBeginInfo& render_pass_begin_info)
{
    linker->vkCmdBeginRenderPass(vulkan_data, &render_pass_begin_info,
        VK_SUBPASS_CONTENTS_INLINE);
}

void gearoenix::render::command::Buffer::set_viewport(
    const VkViewport& viewport)
{
    linker->vkCmdSetViewport(vulkan_data, 0, 1, &viewport);
}

void gearoenix::render::command::Buffer::set_scissor(const VkRect2D& scissor)
{
    linker->vkCmdSetScissor(vulkan_data, 0, 1, &scissor);
}
