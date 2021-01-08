#include "gx-vk-buf-uniform.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/gx-cr-allocator.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"

gearoenix::vulkan::buffer::Uniform::Uniform(
    vulkan::buffer::Buffer&& b,
    const std::size_t size,
    const std::size_t frame_number) noexcept
    : size(size)
    , frame_number(frame_number)
    , base(std::move(b))
{
}

void gearoenix::vulkan::buffer::Uniform::update(const void* src) noexcept
{
    std::memcpy(base.get_allocated_memory().get_data(), src, size);
}

#endif