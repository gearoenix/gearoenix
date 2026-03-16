#include "gx-vk-cmd-buffer.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/allocator/gx-cr-alc-range.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../buffer/gx-vk-buf-buffer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../gx-vk-check.hpp"
#include "../image/gx-vk-img-view.hpp"
#include "../sync/gx-vk-sync-fence.hpp"
#include "gx-vk-cmd-pool.hpp"

#if GX_DEBUG_MODE
#define GX_VK_CMD_BUFF_DEBUG true
#endif

gearoenix::vulkan::command::Buffer::Buffer(std::shared_ptr<Pool>&& p, const Type t)
    : pool(std::move(p))
    , type(t)
{
    VkCommandBufferAllocateInfo info;
    GX_SET_ZERO(info);
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.commandPool = pool->get_vulkan_data();
    info.level = t == Type::Primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    info.commandBufferCount = 1;
    GX_VK_CHK(vkAllocateCommandBuffers(device::Logical::get().get_vulkan_data(), &info, &vulkan_data));
}

gearoenix::vulkan::command::Buffer::Buffer(Buffer&& o) noexcept
    : pool(o.pool)
    , type(o.type)
    , vulkan_data(o.vulkan_data)
{
    o.vulkan_data = nullptr;
}

gearoenix::vulkan::command::Buffer::~Buffer()
{
    if (nullptr != vulkan_data) {
        vkFreeCommandBuffers(device::Logical::get().get_vulkan_data(), pool->get_vulkan_data(), 1, &vulkan_data);
        vulkan_data = nullptr;
    }
}

void gearoenix::vulkan::command::Buffer::begin()
{
    has_record = true;
    VkCommandBufferBeginInfo info;
    GX_SET_ZERO(info);
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    GX_VK_CHK(vkBeginCommandBuffer(vulkan_data, &info));
}

void gearoenix::vulkan::command::Buffer::end()
{
    GX_ASSERT_D(has_record);
    GX_VK_CHK(vkEndCommandBuffer(vulkan_data));
    has_record = false;
}

void gearoenix::vulkan::command::Buffer::copy(const buffer::Buffer& src, const buffer::Buffer& des, const std::vector<VkBufferCopy>& info)
{
    vkCmdCopyBuffer(vulkan_data, src.get_vulkan_data(), des.get_vulkan_data(), static_cast<std::uint32_t>(info.size()), info.data());
}

void gearoenix::vulkan::command::Buffer::copy(const buffer::Buffer& src, const buffer::Buffer& dst)
{
    VkBufferCopy info;
    const auto& src_alc = *src.get_allocated_memory()->get_allocator();
    const auto& dst_alc = *dst.get_allocated_memory()->get_allocator();
    info.size = std::min(src_alc.get_size(), dst_alc.get_size());
    info.srcOffset = src.get_offset();
    info.dstOffset = dst.get_offset();
    vkCmdCopyBuffer(vulkan_data, src.get_vulkan_data(), dst.get_vulkan_data(), 1, &info);
}

void gearoenix::vulkan::command::Buffer::barrier(buffer::Buffer& buff, std::pair<VkAccessFlags, VkPipelineStageFlags> src_state, std::pair<VkAccessFlags, VkPipelineStageFlags> des_state)
{
    const auto& alc = *buff.get_allocated_memory()->get_allocator();
    VkBufferMemoryBarrier info;
    GX_SET_ZERO(info);
    info.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    info.buffer = buff.get_vulkan_data();
    info.offset = buff.get_offset();
    info.size = alc.get_size();
    info.srcAccessMask = src_state.first;
    info.dstAccessMask = des_state.first;
    vkCmdPipelineBarrier(vulkan_data, src_state.second, des_state.second, 0, 0, nullptr, 1, &info, 0, nullptr);
}

// void gearoenix::vulkan::command::Buffer::begin_render_pass_with_info(
//     const VkRenderPassBeginInfo& render_pass_begin_info)
//{
//     Loader::vkCmdBeginRenderPass(vulkan_data, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
// }
//
// void gearoenix::vulkan::command::Buffer::set_viewport(
//     const VkViewport& viewport)
//{
//     Loader::vkCmdSetViewport(vulkan_data, 0, 1, &viewport);
// }
//
// void gearoenix::vulkan::command::Buffer::set_scissor(const VkRect2D& scissor)
//{
//     Loader::vkCmdSetScissor(vulkan_data, 0, 1, &scissor);
// }
//

// void gearoenix::vulkan::command::Buffer::bind_pipeline(VkPipeline pip)
//{
//     Loader::vkCmdBindPipeline(vulkan_data, VK_PIPELINE_BIND_POINT_GRAPHICS, pip);
// }
//
// void gearoenix::vulkan::command::Buffer::bind_descriptor_set(
//     VkPipelineBindPoint pipeline_bind_point,
//     VkPipelineLayout pipeline_layout,
//     uint32_t first_set,
//     const VkDescriptorSet& des_set)
//{
//     Loader::vkCmdBindDescriptorSets(
//         vulkan_data, pipeline_bind_point, pipeline_layout, first_set, 1, &des_set, 0, nullptr);
// }
//
// void gearoenix::vulkan::command::Buffer::bind_vertex_buffers(VkBuffer buf, VkDeviceSize offset)
//{
//     Loader::vkCmdBindVertexBuffers(vulkan_data, 0, 1, &buf, &offset);
// }
//
// void gearoenix::vulkan::command::Buffer::bind_index_buffer(VkBuffer buf, VkDeviceSize offset)
//{
//     Loader::vkCmdBindIndexBuffer(vulkan_data, buf, offset, VK_INDEX_TYPE_UINT32);
// }

void gearoenix::vulkan::command::Buffer::draw_indices(const VkDeviceSize count) { vkCmdDrawIndexed(vulkan_data, static_cast<std::uint32_t>(count), 1, 0, 0, 1); }

void gearoenix::vulkan::command::Buffer::build_acceleration_structure(const VkAccelerationStructureBuildGeometryInfoKHR& info, const VkAccelerationStructureBuildRangeInfoKHR* const* const ranges)
{
    vkCmdBuildAccelerationStructuresKHR(vulkan_data, 1, &info, ranges);
}

const VkCommandBuffer* gearoenix::vulkan::command::Buffer::get_vulkan_data_ptr() const { return &vulkan_data; }

#endif
