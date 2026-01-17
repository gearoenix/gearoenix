#include "gx-vk-buf-manager.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/allocator/gx-cr-alc-range.hpp"
#include "../../core/sync/gx-cr-sync-work-waiter.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-marker.hpp"
#include "../gx-vk-swapchain.hpp"
#include "../queue/gx-vk-qu-queue.hpp"
#include "../sync/gx-vk-sync-fence.hpp"
#include "gx-vk-buf-buffer.hpp"
#include "gx-vk-buf-uniform.hpp"
#include "../command/gx-vk-cmd-manager.hpp"

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

std::vector<std::shared_ptr<gearoenix::vulkan::buffer::Buffer>> gearoenix::vulkan::buffer::Manager::create_each_frame_upload_source() const
{
    const auto& cfg = render::RuntimeConfiguration::get();
    const auto& phs_dev = device::Physical::get();
    const auto dyn_sz = phs_dev.align_size(cfg.get_maximum_dynamic_buffer_size());
    std::vector<std::shared_ptr<Buffer>> result;
    const auto frames_count = Swapchain::get().get_image_views().size();
    result.reserve(frames_count);
    for (auto i = decltype(frames_count){0}; i < frames_count; ++i) {
        result.push_back(std::move(upload_root_buffer->allocate(dyn_sz)));
    }
    return result;
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
    , uploader(new core::sync::WorkWaiter())
{
    uploader->push([this] {
        uploader_queue = std::make_unique<queue::Queue>();
    });
}

gearoenix::vulkan::buffer::Manager::~Manager() = default;

std::shared_ptr<gearoenix::vulkan::buffer::Buffer> gearoenix::vulkan::buffer::Manager::create_static(const std::int64_t size)
{
    return gpu_root_buffer->allocate(size);
}

std::shared_ptr<gearoenix::vulkan::buffer::Buffer> gearoenix::vulkan::buffer::Manager::create_staging(const std::int64_t size)
{
    return upload_root_buffer->allocate(size);
}

std::shared_ptr<gearoenix::vulkan::buffer::Uniform> gearoenix::vulkan::buffer::Manager::create_uniform(const std::int64_t size)
{
    std::vector<std::shared_ptr<Buffer>> cpus(each_frame_upload_source.size());
    for (std::uint64_t fi = 0; fi < each_frame_upload_source.size(); ++fi) {
        cpus[fi] = each_frame_upload_source[fi]->allocate(size);
    }
    auto gpu = each_frame_upload_destination->allocate(size);
    return std::make_shared<Uniform>(std::move(cpus), std::move(gpu));
}

std::shared_ptr<gearoenix::vulkan::buffer::Buffer> gearoenix::vulkan::buffer::Manager::create(
    const std::string& name, const void* const data, const std::int64_t size, const core::job::EndCaller<>& end)
{
    auto cpu = upload_root_buffer->allocate(size);
    if (nullptr == cpu) {
        return nullptr;
    }
    auto gpu = gpu_root_buffer->allocate(size);
    if (nullptr == gpu) {
        return nullptr;
    }
    cpu->write(data, size);
    uploader->push([this, name, cpu = std::move(cpu), gpu, end = end]() mutable {
        auto cmd = std::make_shared<command::Buffer>(command::Manager::get().create(command::Type::Primary));
        GX_VK_MARK(name + "-copy-buffer-cmd", cmd->get_vulkan_data());
        cmd->begin();
        cmd->copy(*cpu, *gpu);
        cmd->end();
        auto fence = std::make_shared<sync::Fence>();
        uploader_queue->submit(*cmd, *fence);
        uploader->push([cpu = std::move(cpu), gpu = std::move(gpu), end = std::move(end), cmd = std::move(cmd), fence = std::move(fence)] {
            fence->wait();
        });
    });
    return gpu;
}

#endif
