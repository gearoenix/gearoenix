#include "gx-vk-buf-static.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../engine/gx-vk-eng-engine.hpp"

gearoenix::vulkan::buffer::Static::Static(
    vulkan::buffer::Buffer&& b,
    const std::size_t size) noexcept
    : size(size)
    , base(std::move(b))
{
}

#endif
