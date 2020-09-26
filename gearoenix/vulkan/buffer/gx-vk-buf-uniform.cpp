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

void gearoenix::vulkan::buffer::Uniform::update(const void* src) noexcept
{
    std::memcpy(data, src, buffer_size);
}

gearoenix::vulkan::buffer::Uniform::Uniform(
    const std::size_t size,
    VkBuffer buffer,
    std::shared_ptr<memory::Memory> memory,
    void* const data,
    engine::Engine* const eng) noexcept
    : render::buffer::Uniform(size, eng)
    , buffer(buffer)
    , memory(std::move(memory))
    , data(data)
{
}

gearoenix::vulkan::buffer::Uniform::~Uniform() noexcept = default;

#endif