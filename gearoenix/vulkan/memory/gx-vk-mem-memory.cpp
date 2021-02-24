#include "gx-vk-mem-memory.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../../math/gx-math-numeric.hpp"
#include "../../platform/gx-plt-log.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../gx-vk-check.hpp"
#include "gx-vk-mem-manager.hpp"

gearoenix::vulkan::memory::Memory::Memory(
    Manager& manager,
    std::shared_ptr<Memory> parent,
    std::shared_ptr<core::Allocator> allocator,
    void* const data,
    const Place place,
    const std::uint32_t type_index,
    VkDeviceMemory vulkan_data) noexcept
    : manager(manager)
    , parent(std::move(parent))
    , allocator(std::move(allocator))
    , data(data)
    , place(place)
    , type_index(type_index)
    , vulkan_data(vulkan_data)
{
}

gearoenix::vulkan::memory::Memory::~Memory() noexcept
{
    if (parent == nullptr) {
        vkFreeMemory(manager.get_logical_device().get_vulkan_data(), vulkan_data, nullptr);
        vulkan_data = nullptr;
    }
}

std::shared_ptr<gearoenix::vulkan::memory::Memory> gearoenix::vulkan::memory::Memory::allocate(const std::size_t size) noexcept
{
    const auto aligned_size = manager.align(size);
    auto alc = allocator->allocate(aligned_size);
    if (nullptr == alc) {
        GX_LOG_D("No more space left in this Vulkan memory")
        return nullptr;
    }
    void* const new_data = (data == nullptr) ? nullptr : reinterpret_cast<void*>(reinterpret_cast<std::size_t>(data) + (alc->get_offset() - allocator->get_offset()));
    return Memory(manager, self.lock(), std::move(alc.value()), new_data, place, type_index, vulkan_data);
}

std::shared_ptr<gearoenix::vulkan::memory::Memory> gearoenix::vulkan::memory::Memory::construct(
    Manager& manager,
    const Place place,
    const std::uint32_t type_index,
    const std::size_t size) noexcept
{
    //    const auto& physical_device = manager.get_logical_device().get_physical_device();
    //    const auto memory_properties = place == Place::Gpu ? VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT : VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    //    const auto index = std::make_pair(physical_device.get_memory_type_index(type_bits, memory_properties), place);
    //    GX_GUARD_LOCK(this)
    //    auto search = memories.find(index);
    //    if (memories.end() == search) {
    //        const auto& cfg = physical_device.get_surface().get_platform_application().get_base().get_configuration().get_render_configuration();
    //        const auto aligned_size = align(place == Place::Gpu ? cfg.get_maximum_gpu_render_memory_size() : cfg.get_maximum_cpu_render_memory_size());

    //        search = memories.find(index);
    //    }
    //    return search->second.allocate(size);

    auto vk_dev = manager.get_logical_device().get_vulkan_data();
    VkMemoryAllocateInfo info;
    GX_SET_ZERO(info)
    info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    info.allocationSize = static_cast<VkDeviceSize>(size);
    info.memoryTypeIndex = type_index;
    VkDeviceMemory vulkan_data = nullptr;
    void* data = nullptr;
    GX_VK_CHK(vkAllocateMemory(vk_dev, &info, nullptr, &vulkan_data))
    if (Place::Cpu == place) {
        GX_VK_CHK(vkMapMemory(vk_dev, vulkan_data, 0, static_cast<VkDeviceSize>(size), 0, &data))
    }
    std::shared_ptr<Memory> result(new Memory(
        manager, nullptr, core::Allocator::construct(size), data, place, type_index, vulkan_data));
    result->self = result;
    return result;
}

#endif
