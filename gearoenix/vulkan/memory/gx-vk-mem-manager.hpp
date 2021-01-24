#ifndef GEAROENIX_VULKAN_MEMORY_MANAGER_HPP
#define GEAROENIX_VULKAN_MEMORY_MANAGER_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-vk-mem-memory.hpp"
#include "gx-vk-mem-place.hpp"
#include <map>

namespace gearoenix::vulkan::device {
struct Logical;
}

namespace gearoenix::vulkan::memory {
struct Manager final {
public:
    GX_GET_CRRF_PRV(device::Logical, logical_device)
private:
    GX_CREATE_GUARD(this)
    /// type-index -> memory
    std::map<std::pair<std::uint32_t, Place>, Memory> memories;

public:
    Manager(const Manager&) = delete;
    Manager(Manager&&) = delete;
    explicit Manager(const device::Logical& logical_device) noexcept;
    ~Manager() noexcept;
    Manager& operator=(const Manager&) = delete;
    Manager& operator=(Manager&&) = delete;
    [[nodiscard]] std::size_t align(std::size_t) const noexcept;
    [[nodiscard]] std::optional<Memory> allocate(std::size_t size, std::uint32_t type_bits, Place place) noexcept;
};
}
#endif
#endif
