#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/gx-cr-singleton.hpp"
#include "gx-vk-mem-place.hpp"

#include <map>
#include <memory>
#include <mutex>

namespace gearoenix::vulkan::memory {
struct Memory;
struct Manager final : core::Singleton<Manager> {
private:
    std::mutex memories_lock;
    std::map<std::uint32_t /* memory type index */, std::weak_ptr<Memory>> memories;

public:
    Manager();
    Manager(Manager&&) = delete;
    Manager(const Manager&) = delete;
    Manager& operator=(Manager&&) = delete;
    Manager& operator=(const Manager&) = delete;
    ~Manager() override;

    [[nodiscard]] std::shared_ptr<Memory> allocate(std::int64_t size, std::int64_t alignment, std::uint32_t type_bits, Place place);
};
}
#endif