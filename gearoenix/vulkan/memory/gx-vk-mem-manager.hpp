#ifndef GEAROENIX_VULKAN_MEMORY_MANAGER_HPP
#define GEAROENIX_VULKAN_MEMORY_MANAGER_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../../core/gx-cr-static.hpp"
#include "../gx-vk-loader.hpp"
#include "gx-vk-mem-place.hpp"
#include <map>

namespace gearoenix::vulkan::device {
class Logical;
}

namespace gearoenix::vulkan::buffer {
class Buffer;
}

namespace gearoenix::vulkan::memory {
class Memory;
class Manager final {
public:
    GX_GET_REFC_PRV(std::shared_ptr<device::Logical>, logical_device)
private:
    std::weak_ptr<Manager> self;
    std::map<Place, int> memories;

    explicit Manager(std::shared_ptr<device::Logical> logical_device) noexcept;

public:
    [[nodiscard]] static std::shared_ptr<Manager> construct(std::shared_ptr<device::Logical> logical_device) noexcept;
    ~Manager() noexcept;

    [[nodiscard]] std::shared_ptr<Memory> allocate(std::size_t size, Place place) noexcept;
};
}
#endif
#endif
