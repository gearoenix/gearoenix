#include "gx-vk-buf-manager.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../platform/gx-plt-application.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"

gearoenix::vulkan::buffer::Manager::Manager(
    memory::Manager& memory_manager,
    engine::Engine& e,
    Buffer&& upload_root_buffer,
    Buffer&& gpu_root_buffer,
    std::vector<Buffer>&& each_frame_upload_source,
    Buffer&& each_frame_upload_destination) noexcept
    : memory_manager(memory_manager)
    , e(e)
    , upload_root_buffer(std::move(upload_root_buffer))
    , gpu_root_buffer(std::move(gpu_root_buffer))
    , each_frame_upload_source(std::move(each_frame_upload_source))
    , each_frame_upload_destination(std::move(each_frame_upload_destination))
{
}

gearoenix::vulkan::buffer::Manager::Manager(Manager&& o) noexcept
    : memory_manager(o.memory_manager)
    , e(o.e)
    , upload_root_buffer(std::move(o.upload_root_buffer))
    , gpu_root_buffer(std::move(o.gpu_root_buffer))
    , each_frame_upload_source(std::move(o.each_frame_upload_source))
    , each_frame_upload_destination(std::move(o.each_frame_upload_destination))
    , upload_buffers(std::move(o.upload_buffers))
    , copy_images(std::move(o.copy_images))
{
}

gearoenix::vulkan::buffer::Manager gearoenix::vulkan::buffer::Manager::construct(
    memory::Manager& memory_manager,
    engine::Engine& e) noexcept
{
    const auto& cfg = e.get_platform_application().get_base().get_configuration().get_render_configuration();
    const auto& phs_dev = e.get_physical_device();
    const auto up_asz = phs_dev.align_size(cfg.get_maximum_cpu_render_memory_size());
    const auto up_sz = (up_asz == cfg.get_maximum_cpu_render_memory_size()) ? cfg.get_maximum_cpu_render_memory_size() : (up_asz - phs_dev.get_max_memory_alignment());
    auto upload_root_buffer = *Buffer::construct("upload-buffer", up_sz, memory::Place::Cpu, memory_manager);
    const auto gpu_asz = phs_dev.align_size(cfg.get_maximum_gpu_buffer_size());
    const auto gpu_sz = (gpu_asz == cfg.get_maximum_gpu_buffer_size()) ? cfg.get_maximum_gpu_buffer_size() : (gpu_asz - phs_dev.get_max_memory_alignment());
    auto gpu_root_buffer = *Buffer::construct("gpu-buffer", gpu_sz, memory::Place::Gpu, memory_manager);
    const auto dyn_sz = phs_dev.align_size(cfg.get_maximum_dynamic_buffer_size());
    std::vector<Buffer> each_frame_upload_source;
    each_frame_upload_source.reserve(e.get_frames_count());
    for (unsigned int i = 0; i < e.get_frames_count(); ++i)
        each_frame_upload_source.push_back(std::move(*upload_root_buffer.allocate(dyn_sz)));
    auto each_frame_upload_destination = *gpu_root_buffer.allocate(dyn_sz);
    return Manager(
        memory_manager,
        e,
        std::move(upload_root_buffer),
        std::move(gpu_root_buffer),
        std::move(each_frame_upload_source),
        std::move(each_frame_upload_destination));
}

gearoenix::vulkan::buffer::Manager::~Manager() noexcept = default;

std::optional<gearoenix::vulkan::buffer::Buffer> gearoenix::vulkan::buffer::Manager::create_static(const std::size_t size) noexcept
{
    return gpu_root_buffer.allocate(size);
}

std::optional<gearoenix::vulkan::buffer::Buffer> gearoenix::vulkan::buffer::Manager::create(
    const void* const data, const std::size_t size, core::sync::EndCallerIgnored end) noexcept
{
    auto o_cpu = upload_root_buffer.allocate(size);
    if (!o_cpu.has_value())
        return std::nullopt;
    auto o_gpu = gpu_root_buffer.allocate(size);
    if (!o_gpu.has_value())
        return std::nullopt;
    auto cpu = std::move(*o_cpu);
    auto gpu = std::move(*o_gpu);
    cpu.write(data, size);
    upload_buffers.emplace_back(gpu.get_allocator().get_offset(), std::move(cpu), std::move(end));
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
            const auto& bf = std::get<1>(up_bf);
            const auto& bf_alc = bf.get_allocator();
            info.size = bf_alc.get_size();
            info.srcOffset = bf_alc.get_offset();
            info.dstOffset = std::get<0>(up_bf);
        }
        cmd.copy(gpu_root_buffer, upload_root_buffer, copy_info);
    }
}

#endif
