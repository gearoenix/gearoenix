#include "gx-vk-mem-manager.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../../math/gx-math-numeric.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-check.hpp"
#include "../gx-vk-instance.hpp"
#include "../gx-vk-surface.hpp"
#include "gx-vk-mem-memory.hpp"

gearoenix::vulkan::memory::Manager::Manager(const engine::Engine& e) noexcept
    : e(e)
{
}

gearoenix::vulkan::memory::Manager::~Manager() noexcept = default;

std::shared_ptr<gearoenix::vulkan::memory::Memory> gearoenix::vulkan::memory::Manager::allocate(
    const std::size_t size, const std::uint32_t type_bits, const Place place) noexcept
{
    const auto& physical_device = e.get_logical_device().get_physical_device();
    const auto memory_properties = place == Place::Gpu ? VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT : VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
    // | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    const auto index = std::make_pair(physical_device.get_memory_type_index(type_bits, memory_properties), place);
    std::shared_ptr<Memory> result;
    GX_GUARD_LOCK(this)
    auto search = memories.find(index);
    if (memories.end() == search) {
        result = Memory::construct(e, place, index.first);
        memories.emplace(index, result);
    } else {
        result = search->second.lock();
        if (nullptr == result) {
            result = Memory::construct(e, place, index.first);
            search->second = result;
        }
    }
    if (nullptr == result)
        return nullptr;
    return result->allocate(size);
}

#endif
