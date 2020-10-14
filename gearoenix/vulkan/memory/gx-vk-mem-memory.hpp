#ifndef GEAROENIX_VULKAN_MEMORY_MEMORY_HPP
#define GEAROENIX_VULKAN_MEMORY_MEMORY_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../../core/gx-cr-static.hpp"
#include "../gx-vk-loader.hpp"
#include "gx-vk-mem-place.hpp"

namespace gearoenix::core {
class Allocator;
}

namespace gearoenix::vulkan::device {
class Logical;
}

namespace gearoenix::vulkan::memory {
class Manager;
class Memory final {
    GX_GET_REFC_PRV(std::shared_ptr<Manager>, manager)
    GX_GET_REFC_PRV(std::shared_ptr<Memory>, parent)
    GX_GET_REFC_PRV(std::shared_ptr<core::Allocator>, allocator)
    GX_GET_CPTR_PRV(void, data)
    GX_GET_VAL_PRV(VkDeviceMemory, vulkan_data, nullptr)
private:
    std::weak_ptr<Memory> self;

    Memory(std::shared_ptr<Manager> manager, std::shared_ptr<Memory> parent, std::shared_ptr<core::Allocator> allocator, void* data) noexcept;

public:
    [[nodiscard]] static std::shared_ptr<Memory> construct(
        std::size_t size, std::uint32_t index, std::shared_ptr<Manager> manager, Place place) noexcept;
    ~Memory() noexcept;
    [[nodiscard]] std::shared_ptr<Memory> allocate(std::size_t size) noexcept;
};
}
#endif
#endif