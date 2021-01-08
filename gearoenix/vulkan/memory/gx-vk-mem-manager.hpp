#ifndef GEAROENIX_VULKAN_MEMORY_MANAGER_HPP
#define GEAROENIX_VULKAN_MEMORY_MANAGER_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"
#include "gx-vk-mem-memory.hpp"
#include "gx-vk-mem-place.hpp"
#include <map>

namespace gearoenix::vulkan::device {
struct Logical;
}

namespace gearoenix::vulkan::memory {
struct Manager final {
public:
    GX_GET_REFC_PRV(std::shared_ptr<device::Logical>, logical_device)
private:
    /// type-index -> memory
    std::map<std::pair<std::uint32_t, Place>, Memory> memories;

public:
    explicit Manager(std::shared_ptr<device::Logical> logical_device) noexcept;
    ~Manager() noexcept;

    [[nodiscard]] Memory allocate(std::size_t size, std::uint32_t type_bits, Place place) noexcept;
};
}
#endif
#endif
