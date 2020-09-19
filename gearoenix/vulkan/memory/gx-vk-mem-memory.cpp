#define GX_VMA_IMPL
#include "gx-vk-mem-memory.hpp"
#ifdef GX_USE_VULKAN
#include "../../system/gx-sys-log.hpp"
#include "../device/gx-vk-dev-logical.hpp"

gearoenix::vulkan::memory::Memory::Memory(std::shared_ptr<Manager> manager, VmaAllocation allocation) noexcept
    : manager(std::move(manager))
    , allocation(allocation)
{
}

gearoenix::vulkan::memory::Memory::~Memory() noexcept
#ifdef GX_DEBUG_VK_MEM
{
    if (not_deleted)
        GXLOGF("There is a bug in memory management.")
}
#else
    = default;
#endif

#endif
