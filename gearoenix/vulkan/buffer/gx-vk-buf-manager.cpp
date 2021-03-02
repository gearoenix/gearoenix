#include "gx-vk-buf-manager.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/gx-cr-allocator.hpp"
#include "../../core/sync/gx-cr-sync-work-waiter.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "gx-vk-buf-buffer.hpp"

std::shared_ptr<gearoenix::vulkan::buffer::Buffer> gearoenix::vulkan::buffer::Manager::create_upload_root_buffer() const noexcept
{
    const auto& cfg = e.get_platform_application().get_base().get_configuration().get_render_configuration();
    const auto& phs_dev = e.get_physical_device();
    const auto up_asz = phs_dev.align_size(cfg.get_maximum_cpu_render_memory_size());
    const auto up_sz = (up_asz == cfg.get_maximum_cpu_render_memory_size()) ? cfg.get_maximum_cpu_render_memory_size() : (up_asz - phs_dev.get_max_memory_alignment());
    return Buffer::construct("upload-buffer", up_sz, memory::Place::Cpu, memory_manager);
}

std::shared_ptr<gearoenix::vulkan::buffer::Buffer> gearoenix::vulkan::buffer::Manager::create_gpu_root_buffer() const noexcept
{
    const auto& cfg = e.get_platform_application().get_base().get_configuration().get_render_configuration();
    const auto& phs_dev = e.get_physical_device();
    const auto gpu_asz = phs_dev.align_size(cfg.get_maximum_gpu_buffer_size());
    const auto gpu_sz = (gpu_asz == cfg.get_maximum_gpu_buffer_size()) ? cfg.get_maximum_gpu_buffer_size() : (gpu_asz - phs_dev.get_max_memory_alignment());
    return Buffer::construct("gpu-buffer", gpu_sz, memory::Place::Gpu, memory_manager);
}

std::vector<std::shared_ptr<gearoenix::vulkan::buffer::Buffer>> gearoenix::vulkan::buffer::Manager::create_each_frame_upload_source() const noexcept
{
    const auto& cfg = e.get_platform_application().get_base().get_configuration().get_render_configuration();
    const auto& phs_dev = e.get_physical_device();
    const auto dyn_sz = phs_dev.align_size(cfg.get_maximum_dynamic_buffer_size());
    std::vector<std::shared_ptr<Buffer>> result;
    result.reserve(e.get_frames_count());
    for (unsigned int i = 0; i < e.get_frames_count(); ++i)
        result.push_back(std::move(upload_root_buffer->allocate(dyn_sz)));
    return result;
}

std::shared_ptr<gearoenix::vulkan::buffer::Buffer> gearoenix::vulkan::buffer::Manager::create_each_frame_upload_destination() const noexcept
{
    const auto& cfg = e.get_platform_application().get_base().get_configuration().get_render_configuration();
    const auto& phs_dev = e.get_physical_device();
    const auto dyn_sz = phs_dev.align_size(cfg.get_maximum_dynamic_buffer_size());
    return gpu_root_buffer->allocate(dyn_sz);
}

gearoenix::vulkan::buffer::Manager::Manager(
    memory::Manager& memory_manager,
    engine::Engine& e) noexcept
    : memory_manager(memory_manager)
    , e(e)
    , upload_root_buffer(create_upload_root_buffer())
    , gpu_root_buffer(create_gpu_root_buffer())
    , each_frame_upload_source(create_each_frame_upload_source())
    , each_frame_upload_destination(create_each_frame_upload_destination())
    , uploader(new core::sync::WorkWaiter())
    , waiter(new core::sync::WorkWaiter())
{
}

gearoenix::vulkan::buffer::Manager::~Manager() noexcept = default;

std::shared_ptr<gearoenix::vulkan::buffer::Buffer> gearoenix::vulkan::buffer::Manager::create_static(const std::size_t size) noexcept
{
    return gpu_root_buffer->allocate(size);
}

std::shared_ptr<gearoenix::vulkan::buffer::Buffer> gearoenix::vulkan::buffer::Manager::create(
    const void* const data,
    const std::size_t size,
    core::sync::EndCaller<Buffer> end) noexcept
{
    auto cpu = upload_root_buffer->allocate(size);
    if (nullptr == cpu)
        return nullptr;
    auto gpu = gpu_root_buffer->allocate(size);
    if (nullptr == gpu)
        return nullptr;
    cpu->write(data, size);
    end.set_data(gpu);
    uploader->push([this, cpu, gpu, end { std::move(end) }] {
        auto cmd = e.get_command_manager().create(command::Type::Primary);
        VkBufferCopy info;
        const auto& bf_alc = *cpu->get_allocator();
        info.size = bf_alc.get_size();
        info.srcOffset = bf_alc.get_offset();
        info.dstOffset = gpu->get_allocator()->get_offset();
        cmd.copy(*gpu_root_buffer, *upload_root_buffer, info);
        // TODO submit and wait on it
    });

    return std::move(gpu);
}

void gearoenix::vulkan::buffer::Manager::do_copies(command::Buffer& cmd) noexcept
{
    decltype(upload_buffers) up_bfs;
    {
        GX_GUARD_LOCK(upload_buffers)
        std::swap(upload_buffers, up_bfs);
    }
    const auto up_sz = up_bfs.size();
    if (up_sz > 0) {
        std::vector<VkBufferCopy> copy_info(up_sz);
        for (std::size_t i = 0; i < up_sz; ++i) {
            const auto& up_bf = up_bfs[i];
            auto& info = copy_info[i];
            const auto& bf_alc = *std::get<1>(up_bf)->get_allocator();
            info.size = bf_alc.get_size();
            info.srcOffset = bf_alc.get_offset();
            info.dstOffset = std::get<0>(up_bf)->get_allocator()->get_offset();
        }
        cmd.copy(*gpu_root_buffer, *upload_root_buffer, copy_info);
    }
}

#endif
