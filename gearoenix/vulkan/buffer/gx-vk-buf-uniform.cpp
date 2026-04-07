#include "gx-vk-buf-uniform.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/allocator/gx-cr-alc-range.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../memory/gx-vk-mem-memory.hpp"
#include "gx-vk-buf-buffer.hpp"

gearoenix::vulkan::buffer::Uniform::Uniform(std::vector<std::shared_ptr<Buffer>>&& cpu, std::shared_ptr<Buffer>&& gpu)
    : cpu(std::move(cpu))
    , gpu(std::move(gpu))
{
}

gearoenix::vulkan::buffer::Uniform::~Uniform() = default;

void gearoenix::vulkan::buffer::Uniform::update(const void* const src, const std::int64_t size)
{
    auto& b = *cpu[core::Singleton<engine::Engine>::get().get_frame_number()];
    GX_ASSERT_D(size <= b.get_allocated_memory()->get_allocator()->get_size());
    b.write(src, size);
}

#endif