#include "gx-vk-mem-manager.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "gx-vk-mem-memory.hpp"

gearoenix::vulkan::memory::Manager::Manager()
    : Singleton(this)
{
}

gearoenix::vulkan::memory::Manager::~Manager() = default;

std::shared_ptr<gearoenix::vulkan::memory::Memory> gearoenix::vulkan::memory::Manager::allocate(
    const std::int64_t size, const std::int64_t alignment, const std::uint32_t type_bits, const Place place)
{
    const auto index = get_memory_type_index(type_bits, place);
    const std::lock_guard _lg(memories_lock);
    auto& root_weak = memories[index];
    auto root = root_weak.lock();
    if (nullptr == root) {
        root = Memory::construct(place, index);
        root_weak = root;
    }
    auto result = root->allocate(size, alignment);
    return result;
}

std::shared_ptr<gearoenix::vulkan::memory::Memory> gearoenix::vulkan::memory::Manager::allocate_dedicated(
    const std::int64_t size, const std::uint32_t type_bits, const Place place)
{
    const auto index = get_memory_type_index(type_bits, place);
    return Memory::construct(place, index, size);
}

std::uint32_t gearoenix::vulkan::memory::Manager::get_memory_type_index(const std::uint32_t type_bits, const Place place)
{
    const auto& physical_device = device::Physical::get();
    if (physical_device.get_unified_memory()) {
        // On unified-memory GPUs, prefer memory that is both device-local and host-accessible.
        // This eliminates the need for staging buffers and CPU-to-GPU copies.
        if (const auto idx = physical_device.get_memory_type_index(
                type_bits, vk::MemoryPropertyFlags(vk::MemoryPropertyFlagBits::eDeviceLocal | vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
            idx.has_value()) {
            return *idx;
        }
    }
    if (place == Place::Gpu) {
        if (const auto idx = physical_device.get_memory_type_index(
                type_bits, vk::MemoryPropertyFlags(vk::MemoryPropertyFlagBits::eDeviceLocal));
            idx.has_value()) {
            return *idx;
        }
    }
    if (const auto idx = physical_device.get_memory_type_index(
            type_bits, vk::MemoryPropertyFlags(vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
        idx.has_value()) {
        return *idx;
    }
    GX_UNEXPECTED;
}

#endif
