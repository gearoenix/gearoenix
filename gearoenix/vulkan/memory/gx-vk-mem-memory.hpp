#ifndef GEAROENIX_VULKAN_MEMORY_MEMORY_HPP
#define GEAROENIX_VULKAN_MEMORY_MEMORY_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../../core/gx-cr-static.hpp"
#include "../gx-vk-loader.hpp"

#include "gx-vk-mem-vma.hpp"

namespace gearoenix::vulkan::device {
class Logical;
}

namespace gearoenix::vulkan::memory {
class Manager;
class Memory final {
    GX_GET_REFC_PRV(std::shared_ptr<Manager>, manager)
    GX_GET_VAL_PRV(VmaAllocation, allocation, nullptr)

public:
    Memory(std::shared_ptr<Manager> manager, VmaAllocation allocation) noexcept;
    ~Memory() noexcept;
};
}
#endif
#endif