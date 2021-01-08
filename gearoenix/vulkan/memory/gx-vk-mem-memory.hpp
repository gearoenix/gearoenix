#ifndef GEAROENIX_VULKAN_MEMORY_MEMORY_HPP
#define GEAROENIX_VULKAN_MEMORY_MEMORY_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"
#include "gx-vk-mem-place.hpp"

namespace gearoenix::core {
struct Allocator;
}

namespace gearoenix::vulkan::device {
struct Logical;
}

namespace gearoenix::vulkan::memory {
struct Manager;
struct Memory final {
    GX_GET_PTR_PRV(Manager, manager)
    GX_GET_PTR_PRV(Memory, parent)
    GX_GET_REFC_PRV(std::shared_ptr<core::Allocator>, allocator)
    GX_GET_PTR_PRV(void, data)
    GX_GET_VAL_PRV(VkDeviceMemory, vulkan_data, nullptr)

private:
    Memory(Manager* manager, Memory* parent, std::shared_ptr<core::Allocator> allocator, void* data) noexcept;

public:
    Memory(const Memory&) noexcept = delete;
    Memory(Memory&&) noexcept;
    [[nodiscard]] static Memory construct(std::size_t size, std::uint32_t index, Manager* manager, Place place) noexcept;
    ~Memory() noexcept;
    [[nodiscard]] Memory allocate(std::size_t size) noexcept;
};
}
#endif
#endif