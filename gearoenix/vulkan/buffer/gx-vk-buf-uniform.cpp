#include "gx-vk-buf-uniform.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/gx-cr-allocator.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "gx-vk-buf-buffer.hpp"
#include <cstring>

gearoenix::vulkan::buffer::Uniform::Uniform(
    const engine::Engine& e,
    std::vector<std::shared_ptr<Buffer>> cpu,
    std::shared_ptr<Buffer> gpu) noexcept
    : e(e)
    , cpu(std::move(cpu))
    , gpu(std::move(gpu))
{
}

gearoenix::vulkan::buffer::Uniform::~Uniform() noexcept = default;

void gearoenix::vulkan::buffer::Uniform::update(const void* const src) noexcept
{
    const auto& b = *cpu[e.get_frame_number()];
    std::memcpy(b.get_allocated_memory()->get_data(), src, b.get_allocator()->get_size());
}

#endif