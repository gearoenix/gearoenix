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
    const std::int64_t size, const std::uint32_t type_bits, const Place place)
{
    const auto& physical_device = device::Physical::get();
    const auto memory_properties = place == Place::Gpu ? VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT : VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
    const auto index = std::make_pair(physical_device.get_memory_type_index(type_bits, memory_properties), place);
    std::shared_ptr<Memory> result;
    const std::lock_guard _lg(memories_lock);
    if (const auto search = memories.find(index); memories.end() == search) {
        result = Memory::construct(place, index.first);
        memories.emplace(index, result);
    } else {
        result = search->second.lock();
        if (nullptr == result) {
            result = Memory::construct(place, index.first);
            search->second = result;
        }
    }
    if (nullptr == result) {
        return nullptr;
    }
    return result->allocate(size);
}

#endif
