#include "gx-vk-buf-manager.hpp"
#ifdef GX_USE_VULKAN
#include "../../core/gx-cr-allocator.hpp"
#include "../../system/gx-sys-application.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../memory/gx-vk-mem-manager.hpp"
#include "gx-vk-buf-buffer.hpp"
#include "gx-vk-buf-static.hpp"
#include "gx-vk-buf-uniform.hpp"
#include <type_traits>

std::vector<std::shared_ptr<gearoenix::vulkan::buffer::Buffer>> gearoenix::vulkan::buffer::Manager::create_per_frame_cpu_root_buffers() const noexcept
{
    const auto count = e->get_frames_count();
    const auto sz = e->get_system_application()->get_configuration().get_render().get_maximum_cpu_render_memory_size() / (count + 1);
    std::vector<std::shared_ptr<Buffer>> result(count);
    for (auto i = decltype(count) { 0 }; i < count; ++i)
        result[i] = Buffer::construct(sz, memory::Place::Cpu, *memory_manager);
    return result;
}

std::shared_ptr<gearoenix::render::buffer::Static> gearoenix::vulkan::buffer::Manager::create_static(
    const std::size_t size, const void* const data) noexcept
{
    auto gpu_buff = gpu_root_buffer->allocate(size);
    if (nullptr == gpu_buff)
        return nullptr;
    auto upload_buff = upload_root_buffer->allocate(size);
    if (nullptr == upload_buff)
        return nullptr;
    auto* const eng = dynamic_cast<engine::Engine*>(e);
    auto uniform = std::make_shared<Uniform>(std::move(upload_buff), size, 0, eng);
    uniform->update(data);
    auto result = std::make_shared<Static>(std::move(gpu_buff), size, eng);
    copy_buffers.emplace_back(std::move(uniform), result);
    return result;
}

gearoenix::vulkan::buffer::Manager::Manager(
    std::shared_ptr<memory::Manager> mm,
    engine::Engine* eng) noexcept
    : render::buffer::Manager(eng)
    , memory_manager(std::move(mm))
    , per_frame_cpu_root_buffers(create_per_frame_cpu_root_buffers())
    , upload_root_buffer(Buffer::construct(per_frame_cpu_root_buffers[0]->get_allocator()->get_size(), memory::Place::Cpu, *memory_manager))
    , gpu_root_buffer(Buffer::construct(e->get_system_application()->get_configuration().get_render().get_maximum_gpu_buffer_size(), memory::Place::Gpu, *memory_manager))
{
}

gearoenix::vulkan::buffer::Manager::~Manager() noexcept = default;

std::shared_ptr<gearoenix::render::buffer::Uniform> gearoenix::vulkan::buffer::Manager::create_uniform(
    const std::size_t size,
    const std::size_t frame_number) noexcept
{
    auto buff = per_frame_cpu_root_buffers[frame_number]->allocate(size);
    if (buff == nullptr)
        return nullptr;
    return std::make_shared<Uniform>(std::move(buff), size, frame_number, dynamic_cast<engine::Engine*>(e));
}

std::shared_ptr<gearoenix::render::buffer::Static> gearoenix::vulkan::buffer::Manager::create_static(
    const std::vector<math::BasicVertex>& values,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>&) noexcept
{
    const std::size_t size = values.size() * sizeof(std::remove_reference<decltype(values)>::type::value_type);
    return create_static(size, values.data());
}

std::shared_ptr<gearoenix::render::buffer::Static> gearoenix::vulkan::buffer::Manager::create_static(
    const std::vector<std::uint32_t>& values,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>&) noexcept
{
    const std::size_t size = values.size() * sizeof(std::remove_reference<decltype(values)>::type::value_type);
    return create_static(size, values.data());
}

#endif
