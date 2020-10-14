#include "gx-vk-buf-uniform.hpp"
#ifdef GX_USE_VULKAN
#include "../../core/gx-cr-allocator.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-check.hpp"
#include "../memory/gx-vk-mem-manager.hpp"
#include "../memory/gx-vk-mem-memory.hpp"
#include "gx-vk-buf-buffer.hpp"

const void* gearoenix::vulkan::buffer::Uniform::get_data() const noexcept
{
    return allocated_buffer->get_allocated_memory()->get_data();
}

void* gearoenix::vulkan::buffer::Uniform::get_data() noexcept
{
    return allocated_buffer->get_allocated_memory()->get_data();
}

gearoenix::vulkan::buffer::Uniform::Uniform(
    std::shared_ptr<vulkan::buffer::Buffer> alc,
    const std::size_t size,
    const std::size_t frame_number,
    engine::Engine* const eng) noexcept
    : render::buffer::Uniform(size, frame_number, eng)
    , allocated_buffer(std::move(alc))
{
}

gearoenix::vulkan::buffer::Uniform::~Uniform() noexcept = default;

void gearoenix::vulkan::buffer::Uniform::update(const void* src) noexcept
{
    std::memcpy(allocated_buffer->get_allocated_memory()->get_data(), src, size);
}

#endif