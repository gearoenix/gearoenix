#include "gx-vk-mem-manager.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../../math/gx-math-numeric.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../gx-vk-check.hpp"
#include "../gx-vk-instance.hpp"
#include "../gx-vk-surface.hpp"

gearoenix::vulkan::memory::Manager::Manager(const device::Logical& ld) noexcept
    : logical_device(ld)
{
}

gearoenix::vulkan::memory::Manager::~Manager() noexcept = default;

std::size_t gearoenix::vulkan::memory::Manager::align(const std::size_t sz) const noexcept
{
    return math::Numeric::align(sz, static_cast<std::size_t>(logical_device.get_physical_device().get_max_memory_alignment()));
}

std::optional<gearoenix::vulkan::memory::Memory> gearoenix::vulkan::memory::Manager::allocate(
    const std::size_t size, const std::uint32_t type_bits, const Place place) noexcept
{
    const auto& physical_device = logical_device.get_physical_device();
    const auto memory_properties = place == Place::Gpu ? VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT : VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    const auto index = std::make_pair(physical_device.get_memory_type_index(type_bits, memory_properties), place);
    GX_GUARD_LOCK(this)
    auto search = memories.find(index);
    if (memories.end() == search) {
        const auto& cfg = physical_device.get_surface().get_platform_application().get_base().get_configuration().get_render_configuration();
        const auto aligned_size = align(place == Place::Gpu ? cfg.get_maximum_gpu_render_memory_size() : cfg.get_maximum_cpu_render_memory_size());
        VkMemoryAllocateInfo info;
        GX_SET_ZERO(info)
        info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        info.allocationSize = static_cast<VkDeviceSize>(aligned_size);
        info.memoryTypeIndex = index.first;
        VkDeviceMemory vulkan_data = nullptr;
        void* data = nullptr;
        GX_VK_CHK(vkAllocateMemory(logical_device.get_vulkan_data(), &info, nullptr, &vulkan_data))
        if (Place::Cpu == place) {
            GX_VK_CHK(vkMapMemory(logical_device.get_vulkan_data(), vulkan_data, 0, static_cast<VkDeviceSize>(aligned_size), 0, &data))
        }
        memories.emplace(index, Memory(this, nullptr, core::Allocator(aligned_size), data, place, index.first, vulkan_data));
        search = memories.find(index);
    }
    return search->second.allocate(size);
}

#endif
