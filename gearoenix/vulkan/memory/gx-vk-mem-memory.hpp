#ifndef GEAROENIX_VULKAN_MEMORY_MEMORY_HPP
#define GEAROENIX_VULKAN_MEMORY_MEMORY_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../../core/gx-cr-static.hpp"
#include "../gx-vk-loader.hpp"

#include "gx-vk-mem-vma.hpp"

#ifdef GX_DEBUG_MODE
#define GX_DEBUG_VK_MEM
#endif

namespace gearoenix::vulkan::device {
class Logical;
}

namespace gearoenix::vulkan::memory {
class Manager;
class Memory final {
    GX_GET_REFC_PRV(std::shared_ptr<Manager>, manager)
    GX_GET_VAL_PRV(VmaAllocation, allocation, nullptr)
    GX_GET_VAL_PRV(VkDeviceMemory, memory, nullptr)
    GX_GET_CVAL_PRV(std::size_t, size)
    GX_GET_CVAL_PRV(std::size_t, offset)
#ifdef GX_DEBUG_VK_MEM
    GX_GETSET_VAL_PRV(bool, not_deleted, true)
#endif
public:
    Memory(std::shared_ptr<Manager> manager, VmaAllocation allocation, const VmaAllocationInfo& info) noexcept;
    ~Memory() noexcept;
};
}
#endif
#endif