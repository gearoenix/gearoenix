#include "gx-vk-buf-static.hpp"
#ifdef GX_USE_VULKAN
#include "../engine/gx-vk-eng-engine.hpp"
#include "../memory/gx-vk-mem-memory.hpp"
#include "gx-vk-buf-buffer.hpp"

gearoenix::vulkan::buffer::Static::Static(
    std::shared_ptr<vulkan::buffer::Buffer> alc,
    std::size_t size,
    engine::Engine* eng) noexcept
    : render::buffer::Static(size, eng)
    , allocated_buffer(std::move(alc))
{
}

gearoenix::vulkan::buffer::Static::~Static() noexcept = default;

#endif
