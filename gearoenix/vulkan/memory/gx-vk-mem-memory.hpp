#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../gx-vk-loader.hpp"
#include "gx-vk-mem-place.hpp"

#include <memory>

namespace gearoenix::core::allocator {
struct Range;
}

namespace gearoenix::vulkan::memory {
struct Memory final {
private:
    const std::shared_ptr<Memory> parent;
    std::weak_ptr<Memory> self;
    const std::shared_ptr<core::allocator::Range> allocator;
    void* const data;
    const Place place;
    const std::uint32_t type_index;
    vk::DeviceMemory vulkan_data;

    Memory(std::shared_ptr<Memory> parent, std::shared_ptr<core::allocator::Range> allocator, void* data, Place place, std::uint32_t type_index, vk::DeviceMemory vulkan_data);

public:
    Memory(Memory&&) = delete;
    Memory(const Memory&) = delete;
    Memory& operator=(Memory&&) = delete;
    Memory& operator=(const Memory&) = delete;
    ~Memory();

    [[nodiscard]] const std::shared_ptr<Memory>& get_parent() const { return parent; }
    [[nodiscard]] const std::shared_ptr<core::allocator::Range>& get_allocator() const { return allocator; }
    [[nodiscard]] void* get_data() const { return data; }
    [[nodiscard]] Place get_place() const { return place; }
    [[nodiscard]] std::uint32_t get_type_index() const { return type_index; }
    [[nodiscard]] vk::DeviceMemory get_vulkan_data() const { return vulkan_data; }

    [[nodiscard]] static std::int64_t align(std::int64_t);
    [[nodiscard]] static std::shared_ptr<Memory> construct(Place, std::uint32_t type_index);
    [[nodiscard]] std::shared_ptr<Memory> allocate(std::int64_t size, std::int64_t alignment);
};
}
#endif
