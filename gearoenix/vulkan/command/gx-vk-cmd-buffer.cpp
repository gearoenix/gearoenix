#include "gx-vk-cmd-buffer.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/allocator/gx-cr-alc-range.hpp"
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../buffer/gx-vk-buf-buffer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../memory/gx-vk-mem-memory.hpp"
#include "gx-vk-cmd-pool.hpp"

gearoenix::vulkan::command::Buffer::Buffer(std::shared_ptr<Pool>&& p, const Type t)
    : pool(std::move(p))
    , type(t)
    , vulkan_data(nullptr)
{
    auto buffers = device::Logical::get().get_device().allocateCommandBuffers(
        vk::CommandBufferAllocateInfo(
            pool->get_vulkan_data(),
            t == Type::Primary ? vk::CommandBufferLevel::ePrimary : vk::CommandBufferLevel::eSecondary,
            1));
    vulkan_data = std::move(buffers[0]);
}

gearoenix::vulkan::command::Buffer::Buffer(Buffer&& o) noexcept
    : pool(o.pool)
    , type(o.type)
    , vulkan_data(std::move(o.vulkan_data))
    , has_record(o.has_record)
{
}

gearoenix::vulkan::command::Buffer::~Buffer() = default;

void gearoenix::vulkan::command::Buffer::begin()
{
    has_record = true;
    vulkan_data.begin(vk::CommandBufferBeginInfo());
}

void gearoenix::vulkan::command::Buffer::end()
{
    GX_ASSERT_D(has_record);
    vulkan_data.end();
    has_record = false;
}

void gearoenix::vulkan::command::Buffer::copy(const buffer::Buffer& src, const buffer::Buffer& des, const std::vector<vk::BufferCopy>& info)
{
    vulkan_data.copyBuffer(src.get_vulkan_data(), des.get_vulkan_data(), info);
}

void gearoenix::vulkan::command::Buffer::copy(const buffer::Buffer& src, const buffer::Buffer& dst)
{
    const auto& src_alc = *src.get_allocated_memory()->get_allocator();
    const auto& dst_alc = *dst.get_allocated_memory()->get_allocator();
    const vk::BufferCopy info(
        src.get_offset(),
        dst.get_offset(),
        std::min(src_alc.get_size(), dst_alc.get_size()));
    vulkan_data.copyBuffer(src.get_vulkan_data(), dst.get_vulkan_data(), info);
}

void gearoenix::vulkan::command::Buffer::barrier(
    const buffer::Buffer& buff,
    const std::pair<vk::AccessFlags, vk::PipelineStageFlags> src_state,
    const std::pair<vk::AccessFlags, vk::PipelineStageFlags> des_state)
{
    const auto& alc = *buff.get_allocated_memory()->get_allocator();
    const vk::BufferMemoryBarrier info(
        src_state.first,
        des_state.first,
        vk::QueueFamilyIgnored,
        vk::QueueFamilyIgnored,
        buff.get_vulkan_data(),
        buff.get_offset(),
        alc.get_size());
    vulkan_data.pipelineBarrier(src_state.second, des_state.second, { }, { }, info, { });
}

void gearoenix::vulkan::command::Buffer::draw_indices(const std::uint64_t count)
{
    vulkan_data.drawIndexed(static_cast<std::uint32_t>(count), 1, 0, 0, 1);
}

#endif
