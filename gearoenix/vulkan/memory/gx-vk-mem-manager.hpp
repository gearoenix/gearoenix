#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-vk-mem-place.hpp"

#include <map>
#include <memory>
#include <mutex>

namespace gearoenix::vulkan::engine {
struct Engine;
}

namespace gearoenix::vulkan::memory {
struct Memory;
struct Manager final {
    GX_GET_CRRF_PRV(engine::Engine, e);

    std::mutex this_lock;
    /// type-index -> memory
    std::map<std::pair<std::uint32_t, Place>, std::weak_ptr<Memory>> memories;

public:
    Manager(const Manager&) = delete;
    Manager(Manager&&) = delete;
    Manager& operator=(const Manager&) = delete;
    Manager& operator=(Manager&&) = delete;
    explicit Manager(const engine::Engine& e);
    ~Manager();
    [[nodiscard]] std::shared_ptr<Memory> allocate(std::uint64_t size, std::uint32_t type_bits, Place place);
};
}
#endif