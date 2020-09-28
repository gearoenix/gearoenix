#include "gx-vk-buf-uniform.hpp"
#ifdef GX_USE_VULKAN
#include "../engine/gx-vk-eng-engine.hpp"

const void* gearoenix::vulkan::buffer::Uniform::get_data() const noexcept
{
    return data;
}

void* gearoenix::vulkan::buffer::Uniform::get_data() noexcept
{
    return data;
}

gearoenix::vulkan::buffer::Uniform::Uniform(
    const std::size_t size,
    const std::uint32_t offset,
    VkBuffer vulkan_data,
    std::shared_ptr<memory::Memory> allocated_memory,
    void* const data,
    engine::Engine* const eng) noexcept
    : render::buffer::Uniform(size, eng)
    , vulkan::buffer::Buffer(vulkan_data, std::move(allocated_memory), data, offset)
{
}

gearoenix::vulkan::buffer::Uniform::~Uniform() noexcept = default;

void gearoenix::vulkan::buffer::Uniform::update(const void* src) noexcept
{
    std::memcpy(data, src, size);
}

#endif