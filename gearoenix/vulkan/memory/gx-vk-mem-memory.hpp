#ifndef GEAROENIX_VULKAN_MEMORY_MEMORY_HPP
#define GEAROENIX_VULKAN_MEMORY_MEMORY_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/gx-cr-allocator.hpp"
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"
#include "gx-vk-mem-place.hpp"

namespace gearoenix::vulkan::memory {
struct Manager;
struct Memory final {
    GX_GET_PTR_PRV(Manager, manager)
    GX_GET_PTR_PRV(Memory, parent)
    GX_GET_CREF_PRV(core::Allocator, allocator)
    GX_GET_PTR_PRV(void, data)
    GX_GET_VAL_PRV(Place, place, Place::Cpu)
    GX_GET_VAL_PRV(std::uint32_t, type_index, 0)
    GX_GET_VAL_PRV(VkDeviceMemory, vulkan_data, nullptr)

public:
    Memory(const Memory&) = delete;
    Memory(
        Manager* manager,
        Memory* parent,
        core::Allocator&& allocator,
        void* data,
        Place place,
        std::uint32_t type_index,
        VkDeviceMemory vulkan_data) noexcept;
    Memory(Memory&&) noexcept;
    ~Memory() noexcept;
    Memory& operator=(const Memory&) = delete;
    Memory& operator=(Memory&&) noexcept;
    [[nodiscard]] std::optional<Memory> allocate(std::size_t size) noexcept;
};
}
#endif
#endif