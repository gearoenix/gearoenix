#include "gx-vk-mem-memory.hpp"
#ifdef GX_USE_VULKAN
#include "../../core/gx-cr-allocator.hpp"
#include "../../math/gx-math-numeric.hpp"
#include "../../system/gx-sys-log.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../gx-vk-check.hpp"
#include "gx-vk-mem-manager.hpp"

gearoenix::vulkan::memory::Memory::Memory(
    std::shared_ptr<Manager> mgr,
    std::shared_ptr<Memory> prt,
    std::shared_ptr<core::Allocator> alc,
    void* const data) noexcept
    : manager(std::move(mgr))
    , parent(std::move(prt))
    , allocator(std::move(alc))
    , data(data)
{
}

std::shared_ptr<gearoenix::vulkan::memory::Memory> gearoenix::vulkan::memory::Memory::construct(
    const std::size_t size,
    const std::uint32_t index,
    std::shared_ptr<Manager> manager,
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
        GX_VK_CHK_L(vkMapMemory(vk_dev, vulkan_data,
            static_cast<VkDeviceSize>(allocator->get_root_offset()),
            static_cast<VkDeviceSize>(size),
            0, &data))
    }
    std::shared_ptr<Memory> result(new Memory(std::move(manager), nullptr, std::move(allocator), data));
    result->vulkan_data = vulkan_data;
    result->self = result;
    return result;
}

gearoenix::vulkan::memory::Memory::~Memory() noexcept
{
    if (parent == nullptr)
        Loader::vkFreeMemory(manager->get_logical_device()->get_vulkan_data(), vulkan_data, nullptr);
}

std::shared_ptr<gearoenix::vulkan::memory::Memory> gearoenix::vulkan::memory::Memory::allocate(const std::size_t size) noexcept
{
    const auto aligned_size = math::Numeric::align(
        size, static_cast<std::size_t>(manager->get_logical_device()->get_physical_device()->get_max_memory_alignment()));
    auto alc = allocator->allocate(aligned_size);
    if (nullptr == alc) {
        GXLOGD("No more space left in this Vulkan memory")
        return nullptr;
    }
    void* const new_data = reinterpret_cast<void*>(reinterpret_cast<std::size_t>(data) + alc->get_offset());
    std::shared_ptr<Memory> result(new Memory(manager, self.lock(), std::move(alc), new_data));
    result->self = result;
    result->vulkan_data = vulkan_data;
    return result;
}

#endif
