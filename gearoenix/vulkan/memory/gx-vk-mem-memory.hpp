#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"
#include "gx-vk-mem-place.hpp"

#include <memory>

namespace gearoenix::core::allocator {
struct Range;
}

namespace gearoenix::vulkan::memory {
struct Memory final {
    GX_GET_REFC_PRV(std::shared_ptr<Memory>, parent);
    GX_GET_CREF_PRV(std::weak_ptr<Memory>, self);
    GX_GET_REFC_PRV(std::shared_ptr<core::allocator::Range>, allocator);
    GX_GET_PTRC_PRV(void, data);
    GX_GET_CVAL_PRV(Place, place);
    GX_GET_CVAL_PRV(std::uint32_t, type_index);
    GX_GET_VAL_PRV(VkDeviceMemory, vulkan_data, nullptr);

    Memory(std::shared_ptr<Memory> parent, std::shared_ptr<core::allocator::Range> allocator, void* data, Place place, std::uint32_t type_index, VkDeviceMemory vulkan_data);

public:
    Memory(Memory&&) = delete;
    Memory(const Memory&) = delete;
    Memory& operator=(Memory&&) = delete;
    Memory& operator=(const Memory&) = delete;
    ~Memory();
    [[nodiscard]] static std::int64_t align(std::int64_t);
    [[nodiscard]] static std::shared_ptr<Memory> construct(Place, std::uint32_t type_index);
    [[nodiscard]] std::shared_ptr<Memory> allocate(std::int64_t size);
};
}
#endif