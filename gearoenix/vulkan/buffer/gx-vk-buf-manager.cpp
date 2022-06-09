#include "gx-vk-buf-manager.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/gx-cr-allocator.hpp"
#include "../../core/sync/gx-cr-sync-work-waiter.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-marker.hpp"
#include "../queue/gx-vk-qu-queue.hpp"
#include "../sync/gx-vk-sync-fence.hpp"
#include "gx-vk-buf-buffer.hpp"
#include "gx-vk-buf-uniform.hpp"

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

std::shared_ptr<gearoenix::vulkan::buffer::Buffer> gearoenix::vulkan::buffer::Manager::create_staging(const std::size_t size) noexcept
{
    return upload_root_buffer->allocate(size);
}

std::shared_ptr<gearoenix::vulkan::buffer::Uniform> gearoenix::vulkan::buffer::Manager::create_uniform(const std::size_t size) noexcept
{
    std::vector<std::shared_ptr<Buffer>> cpus(each_frame_upload_source.size());
    for (std::size_t fi = 0; fi < each_frame_upload_source.size(); ++fi)
        cpus[fi] = each_frame_upload_source[fi]->allocate(size);
    auto gpu = each_frame_upload_destination->allocate(size);
    return std::make_shared<Uniform>(e, std::move(cpus), std::move(gpu));
}

std::shared_ptr<gearoenix::vulkan::buffer::Buffer> gearoenix::vulkan::buffer::Manager::create(
    const std::string& name,
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
    uploader->push([this, name, cpu = std::move(cpu), gpu, end = std::move(end)]() mutable noexcept {
        auto cmd = std::make_shared<command::Buffer>(e.get_command_manager().create(command::Type::Primary));
        GX_VK_MARK(name + "-copy-buffer-cmd", cmd->get_vulkan_data(), e.get_logical_device())
        cmd->begin();
        cmd->copy(*cpu, *gpu);
        cmd->end();
        auto fence = std::make_shared<sync::Fence>(e.get_logical_device());
        e.get_graphic_queue()->submit(*cmd, *fence);
        waiter->push(std::move([this,
                                   cpu = std::move(cpu),
                                   gpu = std::move(gpu),
                                   end = std::move(end),
                                   cmd = std::move(cmd),
                                   fence = std::move(fence)]() mutable noexcept {
            fence->wait();
            uploader->push(std::move([cmd = std::move(cmd)]() mutable noexcept {}));
        }));
    });
    return std::move(gpu);
}

#endif
