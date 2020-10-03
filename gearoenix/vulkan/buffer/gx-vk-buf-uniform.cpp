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
    return data;
}

void* gearoenix::vulkan::buffer::Uniform::get_data() noexcept
{
    return data;
}

gearoenix::vulkan::buffer::Uniform::Uniform(
    std::shared_ptr<vulkan::buffer::Buffer> alc,
    const std::size_t size,
    const std::size_t frame_number,
    engine::Engine* const eng) noexcept
    : render::buffer::Uniform(size, frame_number, eng)
    , allocated_buffer(std::move(alc))
{
    const auto& mem = allocated_buffer->get_allocated_memory();
    // It can be happened one time but some vendors/drivers may complain it.
    GX_VK_CHK_L(vkMapMemory(
        mem->get_manager()->get_logical_device()->get_vulkan_data(),
        mem->get_vulkan_data(),
        static_cast<VkDeviceSize>(mem->get_allocator()->get_root_offset()),
        static_cast<VkDeviceSize>(size),
        0, &data))
}

gearoenix::vulkan::buffer::Uniform::~Uniform() noexcept = default;

void gearoenix::vulkan::buffer::Uniform::update(const void* src) noexcept
{
    std::memcpy(data, src, size);
}

#endif