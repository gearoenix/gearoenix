#define GX_VMA_IMPL
#include "gx-vk-mem-memory.hpp"
#ifdef GX_USE_VULKAN
#include "../device/gx-vk-dev-logical.hpp"

gearoenix::vulkan::memory::Memory::Memory(std::shared_ptr<Manager> manager, VmaAllocation allocation) noexcept
    : manager(std::move(manager))
    , allocation(allocation)
{
}

gearoenix::vulkan::memory::Memory::~Memory() noexcept = default;

#endif
