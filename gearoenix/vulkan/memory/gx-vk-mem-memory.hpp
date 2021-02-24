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
    GX_GET_RRF_PRV(Manager, manager)
    GX_GET_REFC_PRV(std::shared_ptr<Memory>, parent)
    GX_GET_CREF_PRV(std::weak_ptr<Memory>, self)
    GX_GET_REFC_PRV(std::shared_ptr<core::Allocator>, allocator)
    GX_GET_PTRC_PRV(void, data)
    GX_GET_CVAL_PRV(Place, place)
    GX_GET_CVAL_PRV(std::uint32_t, type_index)
    GX_GET_VAL_PRV(VkDeviceMemory, vulkan_data, nullptr)

    Memory(
        Manager& manager,
        std::shared_ptr<Memory> parent,
        std::shared_ptr<core::Allocator> allocator,
        void* data,
        Place place,
        std::uint32_t type_index,
        VkDeviceMemory vulkan_data) noexcept;

public:
    Memory(Memory&&) = delete;
    Memory(const Memory&) = delete;
    Memory& operator=(Memory&&) = delete;
    Memory& operator=(const Memory&) = delete;
    [[nodiscard]] static std::shared_ptr<Memory> construct(
        Manager& manager, Place place, std::uint32_t type_index, std::size_t size) noexcept;
    ~Memory() noexcept;
    [[nodiscard]] std::shared_ptr<Memory> allocate(std::size_t size) noexcept;
};
}
#endif
#endif