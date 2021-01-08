#include "gx-vk-buf-manager.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/gx-cr-allocator.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include <type_traits>

std::vector<gearoenix::vulkan::buffer::Buffer> gearoenix::vulkan::buffer::Manager::create_per_frame_cpu_root_buffers() const noexcept
{
    const auto count = e->get_frames_count();
    const auto sz = (e->get_platform_application()->get_base().get_configuration().get_render_configuration().get_maximum_cpu_render_memory_size() / (count + 1)) - memory_manager->get_logical_device()->get_physical_device()->get_max_memory_alignment();
    std::vector<Buffer> result;
    result.reserve(count);
    for (auto i = decltype(count) { 0 }; i < count; ++i)
        result.push_back(Buffer::construct(sz, memory::Place::Cpu, *memory_manager));
    return result;
}

gearoenix::vulkan::buffer::Static gearoenix::vulkan::buffer::Manager::create_static(
    const std::size_t size,
    const void* const data) noexcept
{
    auto gpu_buff = gpu_root_buffer.allocate(size);
    auto upload_buff = upload_root_buffer.allocate(size);
    auto uniform = Uniform(std::move(upload_buff), size, 0);
    uniform.update(data);
    GX_TODO
    copy_buffers.emplace_back(std::move(uniform), nullptr);
    return Static(std::move(gpu_buff), size, e);
}

gearoenix::vulkan::buffer::Manager::Manager(
    memory::Manager* const memory_manager,
    engine::Engine* const e) noexcept
    : memory_manager(memory_manager)
    , e(e)
    , per_frame_cpu_root_buffers(create_per_frame_cpu_root_buffers())
    , upload_root_buffer(Buffer::construct(per_frame_cpu_root_buffers[0].get_allocator()->get_size(), memory::Place::Cpu, *memory_manager))
    , gpu_root_buffer(Buffer::construct(e->get_platform_application()->get_base().get_configuration().get_render_configuration().get_maximum_gpu_buffer_size(), memory::Place::Gpu, *memory_manager))
{
}

gearoenix::vulkan::buffer::Manager::~Manager() noexcept = default;

gearoenix::vulkan::buffer::Uniform gearoenix::vulkan::buffer::Manager::create_uniform(
    const std::size_t size,
    const std::size_t frame_number) noexcept
{
    return Uniform(per_frame_cpu_root_buffers[frame_number].allocate(size), size, frame_number);
}

#endif
