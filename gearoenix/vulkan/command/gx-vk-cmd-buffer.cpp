#include "gx-vk-cmd-buffer.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../gx-vk-check.hpp"
#include "../sync/gx-vk-sync-fence.hpp"
#include "gx-vk-cmd-pool.hpp"

#ifdef GX_DEBUG_MODE
#define GX_VK_CMD_BUFF_DEBUG
#endif

gearoenix::vulkan::command::Buffer::Buffer(Pool* const pool, const Type t) noexcept
    : pool(pool)
    , type(t)
{
    VkCommandBufferAllocateInfo info;
    GX_SET_ZERO(info)
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.commandPool = pool->get_vulkan_data();
    info.level = t == Type::Primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    info.commandBufferCount = 1;
    GX_VK_CHK_L(vkAllocateCommandBuffers(pool->get_logical_device().get_vulkan_data(), &info, &vulkan_data))
}

gearoenix::vulkan::command::Buffer::~Buffer() noexcept
{
#ifdef GX_VK_CMD_BUFF_DEBUG
    if (nullptr == vulkan_data)
        GX_UNEXPECTED
#endif
    Loader::vkFreeCommandBuffers(
        pool->get_logical_device().get_vulkan_data(),
        pool->get_vulkan_data(), 1, &vulkan_data);
}

void gearoenix::vulkan::command::Buffer::begin() noexcept
{
    VkCommandBufferBeginInfo info;
    GX_SET_ZERO(info)
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    GX_VK_CHK_L(vkBeginCommandBuffer(vulkan_data, &info))
}

void gearoenix::vulkan::command::Buffer::flush() noexcept
{
    sync::Fence fence(pool->get_logical_device());
    GX_VK_CHK_L(vkEndCommandBuffer(vulkan_data))
    VkSubmitInfo submit_info;
    GX_SET_ZERO(submit_info)
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &vulkan_data;
    GX_VK_CHK_L(vkQueueSubmit(
        pool->get_logical_device().get_graphic_queue(),
        1, &submit_info, fence.get_vulkan_data()))
    fence.wait();
}

void gearoenix::vulkan::command::Buffer::end() noexcept
{
    GX_VK_CHK_L(vkEndCommandBuffer(vulkan_data))
}
//
//void gearoenix::vulkan::command::Buffer::copy_buffer(
//    VkBuffer src, VkBuffer des, const VkBufferCopy& region) noexcept
//{
//    Loader::vkCmdCopyBuffer(vulkan_data, src, des, 1, &region);
//}
//
//void gearoenix::vulkan::command::Buffer::begin_render_pass_with_info(
//    const VkRenderPassBeginInfo& render_pass_begin_info) noexcept
//{
//    Loader::vkCmdBeginRenderPass(vulkan_data, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
//}
//
//void gearoenix::vulkan::command::Buffer::set_viewport(
//    const VkViewport& viewport) noexcept
//{
//    Loader::vkCmdSetViewport(vulkan_data, 0, 1, &viewport);
//}
//
//void gearoenix::vulkan::command::Buffer::set_scissor(const VkRect2D& scissor) noexcept
//{
//    Loader::vkCmdSetScissor(vulkan_data, 0, 1, &scissor);
//}
//

void gearoenix::vulkan::command::Buffer::end_render_pass() noexcept
{
    Loader::vkCmdEndRenderPass(vulkan_data);
}

//void gearoenix::vulkan::command::Buffer::bind_pipeline(VkPipeline pip) noexcept
//{
//    Loader::vkCmdBindPipeline(vulkan_data, VK_PIPELINE_BIND_POINT_GRAPHICS, pip);
//}
//
//void gearoenix::vulkan::command::Buffer::bind_descriptor_set(
//    VkPipelineBindPoint pipeline_bind_point,
//    VkPipelineLayout pipeline_layout,
//    uint32_t first_set,
//    const VkDescriptorSet& des_set) noexcept
//{
//    Loader::vkCmdBindDescriptorSets(
//        vulkan_data, pipeline_bind_point, pipeline_layout, first_set, 1, &des_set, 0, nullptr);
//}
//
//void gearoenix::vulkan::command::Buffer::bind_vertex_buffers(VkBuffer buf, VkDeviceSize offset) noexcept
//{
//    Loader::vkCmdBindVertexBuffers(vulkan_data, 0, 1, &buf, &offset);
//}
//
//void gearoenix::vulkan::command::Buffer::bind_index_buffer(VkBuffer buf, VkDeviceSize offset) noexcept
//{
//    Loader::vkCmdBindIndexBuffer(vulkan_data, buf, offset, VK_INDEX_TYPE_UINT32);
//}

void gearoenix::vulkan::command::Buffer::draw_indices(VkDeviceSize count) noexcept
{
    Loader::vkCmdDrawIndexed(vulkan_data, static_cast<std::uint32_t>(count), 1, 0, 0, 1);
}

VkCommandBuffer* gearoenix::vulkan::command::Buffer::get_vulkan_data_ptr() noexcept
{
    return &vulkan_data;
}

#endif
