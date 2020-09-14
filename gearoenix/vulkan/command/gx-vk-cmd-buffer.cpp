#include "gx-vk-cmd-buffer.hpp"
#ifdef GX_USE_VULKAN
#ifdef GX_DEBUG_MODE
#define GX_VK_CMD_BUFF_DEBUG
#endif
#include "../../core/cr-static.hpp"
#include "../../render/command/rnd-cmd-buffer.hpp"
#include "../device/vk-dev-logical.hpp"
#include "../device/vk-dev-physical.hpp"
#include "../gx-vk-check.hpp"
#include "../gx-vk-instance.hpp"
#include "../sync/vk-sync-fence.hpp"
#include "gx-vk-cmd-pool.hpp"

gearoenix::render::command::Buffer::Buffer(std::shared_ptr<Pool> p) noexcept
    : pool(std::move(p))
{
    device::Logical* dev = pool->get_logical_device();
    VkDevice vkdev = dev->get_vulkan_data();
    device::Physical* pdev = dev->get_physical_device();
    Instance* ins = pdev->get_instance();
    VkCommandBufferAllocateInfo cmd_buf_allocate_info;
    GX_SET_ZERO(cmd_buf_allocate_info)
    cmd_buf_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmd_buf_allocate_info.commandPool = pool->get_vulkan_data();
    cmd_buf_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmd_buf_allocate_info.commandBufferCount = 1;
    VKC(l->vkAllocateCommandBuffers(vkdev, &cmd_buf_allocate_info, &vulkan_data));
}

gearoenix::render::command::Buffer::~Buffer()
{
    device::Logical* d = pool->get_logical_device();
    VkDevice vd = d->get_vulkan_data();
    VkCommandPool vp = pool->get_vulkan_data();
    linker->vkFreeCommandBuffers(vd, vp, 1, &vulkan_data);
}

void gearoenix::render::command::Buffer::begin()
{
    VkCommandBufferBeginInfo cmd_buf_info;
    setz(cmd_buf_info);
    cmd_buf_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    VKC(linker->vkBeginCommandBuffer(vulkan_data, &cmd_buf_info));
}

void gearoenix::render::command::Buffer::record(const std::shared_ptr<Buffer>& o)
{
}

void gearoenix::render::command::Buffer::copy_buffer(
    VkBuffer src, VkBuffer des, const VkBufferCopy& region)
{
    linker->vkCmdCopyBuffer(vulkan_data, src, des, 1, &region);
}

void gearoenix::render::command::Buffer::flush()
{
    device::Logical* d = pool->get_logical_device();
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

void gearoenix::render::command::Buffer::end()
{
    VKC(linker->vkEndCommandBuffer(vulkan_data));
}

void gearoenix::render::command::Buffer::bind(const std::shared_ptr<pipeline::ResourceSet>& r)
{
}

void gearoenix::render::command::Buffer::end_render_pass()
{
    linker->vkCmdEndRenderPass(vulkan_data);
}

void gearoenix::render::command::Buffer::bind_pipeline(VkPipeline pip)
{
    linker->vkCmdBindPipeline(vulkan_data, VK_PIPELINE_BIND_POINT_GRAPHICS, pip);
}

void gearoenix::render::command::Buffer::bind_descriptor_set(
    VkPipelineBindPoint pipeline_bind_point,
    VkPipelineLayout pipeline_layout,
    uint32_t first_set,
    const VkDescriptorSet& desset)
{
    linker->vkCmdBindDescriptorSets(vulkan_data, pipeline_bind_point, pipeline_layout, first_set, 1, &desset, 0, nullptr);
}

void gearoenix::render::command::Buffer::bind_vertex_buffers(VkBuffer buf, VkDeviceSize offset)
{
    linker->vkCmdBindVertexBuffers(vulkan_data, 0, 1, &buf, &offset);
}

void gearoenix::render::command::Buffer::bind_index_buffer(VkBuffer buf, VkDeviceSize offset)
{
    linker->vkCmdBindIndexBuffer(vulkan_data, buf, offset, VK_INDEX_TYPE_UINT32);
}

void gearoenix::render::command::Buffer::draw_indexed(VkDeviceSize count)
{
    linker->vkCmdDrawIndexed(vulkan_data, count, 1, 0, 0, 1);
}
#endif
