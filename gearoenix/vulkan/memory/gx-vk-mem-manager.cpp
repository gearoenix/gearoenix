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
    const auto memory_properties = place == Place::Gpu ? VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT :(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    const auto index = std::make_pair(physical_device.get_memory_type_index(type_bits, memory_properties), place);
    const std::lock_guard _lg(memories_lock);
    auto& root_weak = memories[index];
    auto root = root_weak.lock();
    if (nullptr == root) {
        root = Memory::construct(place, index.first);
        root_weak = root;
    }
    return root->allocate(size);
}

#endif
