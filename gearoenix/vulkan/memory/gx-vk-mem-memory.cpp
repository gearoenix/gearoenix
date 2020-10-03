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
    std::shared_ptr<core::Allocator> alc) noexcept
    : manager(std::move(mgr))
    , parent(std::move(prt))
    , allocator(std::move(alc))
{
}

std::shared_ptr<gearoenix::vulkan::memory::Memory> gearoenix::vulkan::memory::Memory::construct(
    const std::size_t size,
    const std::uint32_t index,
    std::shared_ptr<Manager> manager) noexcept
{
    const auto* const logical_device = manager->get_logical_device().get();
    const auto aligned_size = math::Numeric::align(
        size, static_cast<std::size_t>(logical_device->get_physical_device()->get_max_memory_alignment()));
    std::shared_ptr<Memory> result(new Memory(std::move(manager), nullptr, core::Allocator::construct(aligned_size, 0, 0)));
    result->self = result;
    VkMemoryAllocateInfo info;
    GX_SET_ZERO(info)
    info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    info.allocationSize = static_cast<VkDeviceSize>(aligned_size);
    info.memoryTypeIndex = index;
    GX_VK_CHK_L(vkAllocateMemory(logical_device->get_vulkan_data(), &info, nullptr, &(result->vulkan_data)))
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
    std::shared_ptr<Memory> result(new Memory(manager, self.lock(), std::move(alc)));
    result->self = result;
    result->vulkan_data = vulkan_data;
    return result;
}

#endif
