#include "gx-vk-mem-memory.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/gx-cr-allocator.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../../math/gx-math-numeric.hpp"
#include "../../platform/gx-plt-log.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../gx-vk-check.hpp"
#include "gx-vk-mem-manager.hpp"

gearoenix::vulkan::memory::Memory::Memory(
    Manager* const mgr,
    Memory* const prt,
    std::shared_ptr<core::Allocator> alc,
    void* const data) noexcept
    : manager(mgr)
    , parent(prt)
    , allocator(std::move(alc))
    , data(data)
{
}

gearoenix::vulkan::memory::Memory::Memory(Memory&&) noexcept = default;

gearoenix::vulkan::memory::Memory gearoenix::vulkan::memory::Memory::construct(
    const std::size_t size,
    const std::uint32_t index,
    Manager* const manager,
    const Place place) noexcept
{
    const auto* const logical_device = manager->get_logical_device().get();
    const auto aligned_size = math::Numeric::align(
        size, static_cast<std::size_t>(logical_device->get_physical_device()->get_max_memory_alignment()));
    auto allocator = core::Allocator::construct(aligned_size, 0, 0);
    VkMemoryAllocateInfo info;
    GX_SET_ZERO(info)
    info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    info.allocationSize = static_cast<VkDeviceSize>(aligned_size);
    info.memoryTypeIndex = index;
    auto vk_dev = logical_device->get_vulkan_data();
    VkDeviceMemory vulkan_data = nullptr;
    void* data = nullptr;
    GX_VK_CHK_L(vkAllocateMemory(vk_dev, &info, nullptr, &vulkan_data))
    if (Place::Cpu == place) {
        GX_VK_CHK_L(vkMapMemory(vk_dev, vulkan_data, 0, static_cast<VkDeviceSize>(aligned_size), 0, &data))
    }
    Memory result(manager, nullptr, std::move(allocator), data);
    result.vulkan_data = vulkan_data;
    return result;
}

gearoenix::vulkan::memory::Memory::~Memory() noexcept
{
    if (parent == nullptr)
        Loader::vkFreeMemory(manager->get_logical_device()->get_vulkan_data(), vulkan_data, nullptr);
}

gearoenix::vulkan::memory::Memory gearoenix::vulkan::memory::Memory::allocate(const std::size_t size) noexcept
{
    const auto aligned_size = math::Numeric::align(
        size, static_cast<std::size_t>(manager->get_logical_device()->get_physical_device()->get_max_memory_alignment()));
    auto alc = allocator->allocate(aligned_size);
    if (nullptr == alc) {
        GX_LOG_F("No more space left in this Vulkan memory")
    }
    void* const new_data = reinterpret_cast<void*>(reinterpret_cast<std::size_t>(data) + alc->get_offset());
    Memory result(manager, this, std::move(alc), new_data);
    result.vulkan_data = vulkan_data;
    return result;
}

#endif
