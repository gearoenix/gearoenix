#include "gx-vk-buf-manager.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/allocator/gx-cr-alc-range.hpp"
#include "../../core/sync/gx-cr-sync-work-waiter.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../command/gx-vk-cmd-manager.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-marker.hpp"
#include "../queue/gx-vk-qu-queue.hpp"
#include "../sync/gx-vk-sync-fence.hpp"
#include "gx-vk-buf-buffer.hpp"
#include "gx-vk-buf-uniform.hpp"

std::shared_ptr<gearoenix::vulkan::buffer::Buffer> gearoenix::vulkan::buffer::Manager::create_upload_root_buffer() const
{
    const auto& cfg = render::RuntimeConfiguration::get();
    const auto& phs_dev = device::Physical::get();
    const auto up_asz = phs_dev.align_size(cfg.get_maximum_cpu_render_memory_size());
    const auto up_sz = (up_asz == cfg.get_maximum_cpu_render_memory_size()) ? cfg.get_maximum_cpu_render_memory_size() : (up_asz - phs_dev.get_max_memory_alignment());
    return Buffer::construct("upload-buffer", up_sz, memory::Place::Cpu);
}

std::shared_ptr<gearoenix::vulkan::buffer::Buffer> gearoenix::vulkan::buffer::Manager::create_gpu_root_buffer() const
{
    const auto& cfg = render::RuntimeConfiguration::get();
    const auto& phs_dev = device::Physical::get();
    const auto gpu_asz = phs_dev.align_size(cfg.get_maximum_gpu_buffer_size());
    const auto gpu_sz = (gpu_asz == cfg.get_maximum_gpu_buffer_size()) ? cfg.get_maximum_gpu_buffer_size() : (gpu_asz - phs_dev.get_max_memory_alignment());
    return Buffer::construct("gpu-buffer", gpu_sz, memory::Place::Gpu);
}

std::array<std::shared_ptr<gearoenix::vulkan::buffer::Buffer>, gearoenix::vulkan::frames_in_flight> gearoenix::vulkan::buffer::Manager::create_each_frame_upload_source() const
{
    const auto& cfg = render::RuntimeConfiguration::get();
    const auto& phs_dev = device::Physical::get();
    const auto dyn_sz = phs_dev.align_size(cfg.get_maximum_dynamic_buffer_size());
    return { upload_root_buffer->allocate(dyn_sz), upload_root_buffer->allocate(dyn_sz), upload_root_buffer->allocate(dyn_sz) };
}

std::shared_ptr<gearoenix::vulkan::buffer::Buffer> gearoenix::vulkan::buffer::Manager::create_each_frame_upload_destination() const
{
    const auto& cfg = render::RuntimeConfiguration::get();
    const auto& phs_dev = device::Physical::get();
    const auto dyn_sz = phs_dev.align_size(cfg.get_maximum_dynamic_buffer_size());
    return gpu_root_buffer->allocate(dyn_sz);
}

gearoenix::vulkan::buffer::Manager::Manager()
    : Singleton(this)
    , upload_root_buffer(create_upload_root_buffer())
    , gpu_root_buffer(create_gpu_root_buffer())
    , each_frame_upload_source(create_each_frame_upload_source())
    , each_frame_upload_destination(create_each_frame_upload_destination())
{
}

gearoenix::vulkan::buffer::Manager::~Manager() = default;

std::shared_ptr<gearoenix::vulkan::buffer::Buffer> gearoenix::vulkan::buffer::Manager::create_static(const std::int64_t size) { return gpu_root_buffer->allocate(size); }

std::shared_ptr<gearoenix::vulkan::buffer::Buffer> gearoenix::vulkan::buffer::Manager::create_staging(const std::int64_t size) { return upload_root_buffer->allocate(size); }

std::shared_ptr<gearoenix::vulkan::buffer::Uniform> gearoenix::vulkan::buffer::Manager::create_uniform(const std::int64_t size)
{
    std::vector<std::shared_ptr<Buffer>> cpus(each_frame_upload_source.size());
    for (std::uint64_t fi = 0; fi < each_frame_upload_source.size(); ++fi) {
        cpus[fi] = each_frame_upload_source[fi]->allocate(size);
    }
    auto gpu = each_frame_upload_destination->allocate(size);
    return std::make_shared<Uniform>(std::move(cpus), std::move(gpu));
}

std::shared_ptr<gearoenix::vulkan::buffer::Buffer> gearoenix::vulkan::buffer::Manager::create(const std::string& name, const void* const data, const std::int64_t size, core::job::EndCaller<>&& end)
{
    auto cpu = upload_root_buffer->allocate(size);
    if (nullptr == cpu) {
        return nullptr;
    }
    cpu->write(data, size);

    auto gpu = gpu_root_buffer->allocate(size);
    if (nullptr == gpu) {
        return nullptr;
    }

    auto fence = std::make_shared<sync::Fence>();

    auto cmd = command::Manager::create_thread_independent();
    GX_VK_MARK(name + "-copy-buffer-cmd", cmd->get_vulkan_data());
    cmd->begin();
    cmd->copy(*cpu, *gpu);
    cmd->end();
    queue::Queue::get().submit(*cmd, *fence);

    core::job::send_job_to_pool([end = std::move(end), fence = std::move(fence), gpu, cpu = std::move(cpu), cmd = std::move(cmd)] {
        fence->wait();
        (void)end;
        (void)gpu;
        (void)cpu;
        (void)cmd;
    });

    return gpu;
}

void gearoenix::vulkan::buffer::Manager::upload_dynamics(const VkCommandBuffer vk_cmd)
{
    const auto frame_number = Singleton<engine::Engine>::get().get_frame_number();
    const auto& src = *each_frame_upload_source[frame_number];
    const auto& dst = *each_frame_upload_destination;

    const auto& src_alloc = *src.get_allocated_memory()->get_allocator();
    const auto& dst_alloc = *dst.get_allocated_memory()->get_allocator();

    VkBufferCopy region {};
    region.srcOffset = static_cast<VkDeviceSize>(src_alloc.get_offset());
    region.dstOffset = static_cast<VkDeviceSize>(dst_alloc.get_offset());
    region.size = static_cast<VkDeviceSize>(src_alloc.get_size());

    vkCmdCopyBuffer(vk_cmd, src.get_vulkan_data(), dst.get_vulkan_data(), 1, &region);

    VkBufferMemoryBarrier2 barrier {};
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;
    barrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
    barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
    barrier.dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
    barrier.dstAccessMask = VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT | VK_ACCESS_2_UNIFORM_READ_BIT | VK_ACCESS_2_SHADER_STORAGE_READ_BIT | VK_ACCESS_2_SHADER_READ_BIT;
    barrier.buffer = dst.get_vulkan_data();
    barrier.offset = region.dstOffset;
    barrier.size = region.size;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    VkDependencyInfo dep {};
    dep.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dep.bufferMemoryBarrierCount = 1;
    dep.pBufferMemoryBarriers = &barrier;

    vkCmdPipelineBarrier2(vk_cmd, &dep);
}

#endif
