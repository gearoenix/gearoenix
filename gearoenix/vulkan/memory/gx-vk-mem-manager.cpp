#include "gx-vk-mem-manager.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../math/gx-math-numeric.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../gx-vk-instance.hpp"
#include "../gx-vk-surface.hpp"

gearoenix::vulkan::memory::Manager::Manager(std::shared_ptr<device::Logical> ld) noexcept
    : logical_device(std::move(ld))
{
}

gearoenix::vulkan::memory::Manager::~Manager() noexcept
{
    memories.clear();
}

gearoenix::vulkan::memory::Memory gearoenix::vulkan::memory::Manager::allocate(
    const std::size_t size, const std::uint32_t type_bits, const Place place) noexcept
{
    const auto& physical_device = logical_device->get_physical_device();
    const auto memory_properties = place == Place::Gpu ? VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT : VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    const auto index = std::make_pair(physical_device->get_memory_type_index(type_bits, memory_properties), place);
    auto search = memories.find(index);
    if (memories.end() == search) {
        const auto& cfg = physical_device->get_surface()->get_platform_application()->get_base().get_configuration().get_render_configuration();
        const auto sz = place == Place::Gpu ? cfg.get_maximum_gpu_render_memory_size() : cfg.get_maximum_cpu_render_memory_size();
        memories.emplace(index, Memory::construct(sz, index.first, this, place));
        search = memories.find(index);
    }
    return search->second.allocate(size);
}
#endif
