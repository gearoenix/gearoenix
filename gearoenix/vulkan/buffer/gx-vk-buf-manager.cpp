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
#include "../memory/gx-vk-mem-memory.hpp"
#include "../queue/gx-vk-qu-queue.hpp"
#include "../sync/gx-vk-sync-fence.hpp"
#include "gx-vk-buf-buffer.hpp"
#include "gx-vk-buf-uniform.hpp"

namespace {
std::atomic last_staging = 0;
}

std::shared_ptr<gearoenix::vulkan::buffer::Buffer> gearoenix::vulkan::buffer::Manager::create_host_root() const
{
    if (is_unified_memory) {
        return device_root;
    }

    const auto sz = Buffer::get_max_cpu_needed_size();
    auto result = Buffer::construct("upload-buffer", sz, memory::Place::Cpu);
    GX_ASSERT_D(result);
    return result;
}

std::shared_ptr<gearoenix::vulkan::buffer::Buffer> gearoenix::vulkan::buffer::Manager::create_device_root() const
{
    const auto sz = Buffer::get_max_gpu_needed_size();
    auto result = Buffer::construct("gpu-buffer", sz, memory::Place::Gpu);
    GX_ASSERT_D(result);
    return result;
}

std::array<std::shared_ptr<gearoenix::vulkan::buffer::Buffer>, gearoenix::vulkan::frames_in_flight> gearoenix::vulkan::buffer::Manager::create_each_frame() const
{
    const auto& cfg = render::RuntimeConfiguration::get();
    const auto dyn_sz = cfg.get_maximum_allowed_dynamic_buffer_size();
    return { host_root->allocate(dyn_sz), host_root->allocate(dyn_sz) };
}

std::shared_ptr<gearoenix::vulkan::buffer::Buffer> gearoenix::vulkan::buffer::Manager::create_each_frame_destination() const
{
    if (is_unified_memory) {
        // We don't need destination buffer in unified memory systems.
        return nullptr;
    }
    const auto& cfg = render::RuntimeConfiguration::get();
    const auto dyn_sz = cfg.get_maximum_allowed_dynamic_buffer_size();
    return device_root->allocate(dyn_sz);
}

gearoenix::vulkan::buffer::Manager::Manager()
    : Singleton<Manager>(this)
    , is_unified_memory(device::Physical::get().get_unified_memory())
    , device_root(create_device_root())
    , host_root(create_host_root())
    , each_frame(create_each_frame())
    , each_frame_destination(create_each_frame_destination())
{
}

gearoenix::vulkan::buffer::Manager::~Manager() = default;

std::shared_ptr<gearoenix::vulkan::buffer::Buffer> gearoenix::vulkan::buffer::Manager::create_static(const std::int64_t size)
{
    return device_root->allocate(size);
}

std::shared_ptr<gearoenix::vulkan::buffer::Buffer> gearoenix::vulkan::buffer::Manager::create_staging(const std::int64_t size)
{
    const auto id = last_staging.fetch_add(1, std::memory_order_relaxed);
    const auto name = "staging-buffer-" + std::to_string(id);
    auto result = Buffer::construct(name, size, memory::Place::Cpu, true);
    GX_ASSERT_D(result);
    return result;
}

std::shared_ptr<gearoenix::vulkan::buffer::Uniform> gearoenix::vulkan::buffer::Manager::create_uniform_vk(const std::int64_t size)
{
    const auto alignment = static_cast<std::int64_t>(device::Physical::get().get_properties().limits.minUniformBufferOffsetAlignment);
    std::vector<std::shared_ptr<Buffer>> cpus(each_frame.size());
    for (std::uint64_t fi = 0; fi < each_frame.size(); ++fi) {
        cpus[fi] = each_frame[fi]->allocate(size, alignment);
    }
    auto gpu = is_unified_memory ? nullptr : each_frame_destination->allocate(size, alignment);
    return std::make_shared<Uniform>(static_cast<std::size_t>(size), std::move(cpus), std::move(gpu));
}

std::shared_ptr<gearoenix::render::buffer::Uniform> gearoenix::vulkan::buffer::Manager::create_uniform(const std::size_t size)
{
    return create_uniform_vk(static_cast<std::int64_t>(size));
}

std::shared_ptr<gearoenix::vulkan::buffer::Buffer> gearoenix::vulkan::buffer::Manager::create([[maybe_unused]] const std::string& name, const void* const data, const std::int64_t size, core::job::EndCaller<>&& end)
{
    auto gpu = device_root->allocate(size);
    if (nullptr == gpu) {
        return nullptr;
    }

    // On unified-memory GPUs, the GPU buffer is directly host-writable.
    // Skip staging buffer allocation, copy commands, and fence synchronisation entirely.
    if (is_unified_memory) {
        GX_ASSERT_D(gpu->get_allocated_memory()->get_data() != nullptr);
        gpu->write(data, size);
        (void)end;
        return gpu;
    }

    auto cpu = create_staging(size);
    if (nullptr == cpu) {
        return nullptr;
    }
    cpu->write(data, size);

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

void gearoenix::vulkan::buffer::Manager::upload_dynamics(const vk::CommandBuffer vk_cmd)
{
    if (is_unified_memory) {
        return; // No need to upload
    }

    const auto frame_number = Singleton<engine::Engine>::get().get_frame_number();
    const auto& src = *each_frame[frame_number];
    const auto& dst = *each_frame_destination;

    const auto& src_alloc = *src.get_allocated_memory()->get_allocator();

    const vk::BufferCopy region(
        static_cast<vk::DeviceSize>(src.get_offset()),
        static_cast<vk::DeviceSize>(dst.get_offset()),
        static_cast<vk::DeviceSize>(src_alloc.get_size()));

    vk_cmd.copyBuffer(src.get_vulkan_data(), dst.get_vulkan_data(), region);

    vk::BufferMemoryBarrier2 barrier;
    barrier.srcStageMask = vk::PipelineStageFlagBits2::eTransfer;
    barrier.srcAccessMask = vk::AccessFlagBits2::eTransferWrite;
    barrier.dstStageMask = vk::PipelineStageFlagBits2::eVertexInput
        | vk::PipelineStageFlagBits2::eVertexShader
        | vk::PipelineStageFlagBits2::eFragmentShader
        | vk::PipelineStageFlagBits2::eComputeShader;
    barrier.dstAccessMask = vk::AccessFlagBits2::eVertexAttributeRead
        | vk::AccessFlagBits2::eUniformRead
        | vk::AccessFlagBits2::eShaderStorageRead
        | vk::AccessFlagBits2::eShaderRead;
    barrier.buffer = dst.get_vulkan_data();
    barrier.offset = region.dstOffset;
    barrier.size = region.size;
    barrier.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
    barrier.dstQueueFamilyIndex = vk::QueueFamilyIgnored;

    vk::DependencyInfo dep;
    dep.setBufferMemoryBarriers(barrier);

    vk_cmd.pipelineBarrier2(dep);
}

#endif
