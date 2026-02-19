#include "gx-vk-buf-uniform.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/allocator/gx-cr-alc-range.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "gx-vk-buf-buffer.hpp"

#include <cstring>

gearoenix::vulkan::buffer::Uniform::Uniform(std::vector<std::shared_ptr<Buffer>> cpu, std::shared_ptr<Buffer> gpu)
    : cpu(std::move(cpu))
    , gpu(std::move(gpu))
{
}

gearoenix::vulkan::buffer::Uniform::~Uniform() = default;

void gearoenix::vulkan::buffer::Uniform::update(const void* const src)
{
    auto& b = *cpu[core::Singleton<engine::Engine>::get().get_frame_number()];
    b.write(src, b.get_allocated_memory()->get_allocator()->get_size());
}

#endif