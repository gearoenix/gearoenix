#include "gx-vk-buf-static.hpp"
#ifdef GX_USE_VULKAN
#include "../engine/gx-vk-eng-engine.hpp"
#include "../memory/gx-vk-mem-memory.hpp"

gearoenix::vulkan::buffer::Static::Static(
    std::size_t size,
    std::uint32_t offset,
    VkBuffer vulkan_data,
    std::shared_ptr<memory::Memory> allocated_memory,
    engine::Engine* eng) noexcept
    : render::buffer::Static(size, eng)
    , vulkan::buffer::Buffer(vulkan_data, std::move(allocated_memory), nullptr, offset)
{
}

gearoenix::vulkan::buffer::Static::~Static() noexcept = default;

#endif
