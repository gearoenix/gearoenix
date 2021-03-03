#include "gx-vk-mem-memory.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/gx-cr-allocator.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../../math/gx-math-numeric.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../../platform/gx-plt-log.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-check.hpp"
#include "gx-vk-mem-manager.hpp"

gearoenix::vulkan::memory::Memory::Memory(
    const engine::Engine& e,
    std::shared_ptr<Memory> parent,
    std::shared_ptr<core::Allocator> allocator,
    void* const data,
    const Place place,
    const std::uint32_t type_index,
    VkDeviceMemory vulkan_data) noexcept
    : e(e)
    , parent(std::move(parent))
    , allocator(std::move(allocator))
    , data(data)
    , place(place)
    , type_index(type_index)
    , vulkan_data(vulkan_data)
{
}

std::size_t gearoenix::vulkan::memory::Memory::align(const engine::Engine& e, const std::size_t sz) noexcept
{
    return math::Numeric::align(sz, static_cast<std::size_t>(e.get_logical_device().get_physical_device().get_max_memory_alignment()));
}

gearoenix::vulkan::memory::Memory::~Memory() noexcept
{
    if (parent == nullptr) {
        vkFreeMemory(e.get_logical_device().get_vulkan_data(), vulkan_data, nullptr);
        vulkan_data = nullptr;
    }
}

std::shared_ptr<gearoenix::vulkan::memory::Memory> gearoenix::vulkan::memory::Memory::allocate(const std::size_t size) noexcept
{
    const auto aligned_size = align(e, size);
    auto alc = allocator->allocate(aligned_size);
    if (nullptr == alc) {
        GX_LOG_D("No more space left in this Vulkan memory")
        return nullptr;
    }
    void* const new_data = (data == nullptr) ? nullptr : reinterpret_cast<void*>(reinterpret_cast<std::size_t>(data) + (alc->get_offset() - allocator->get_offset()));
    std::shared_ptr<Memory> result(new Memory(e, self.lock(), std::move(alc), new_data, place, type_index, vulkan_data));
    result->self = result;
    return result;
}

std::shared_ptr<gearoenix::vulkan::memory::Memory> gearoenix::vulkan::memory::Memory::construct(
    const engine::Engine& e,
    const Place place,
    const std::uint32_t type_index) noexcept
{
    const auto& cfg = e.get_platform_application().get_base().get_configuration().get_render_configuration();
    const auto aligned_size = align(e, place == Place::Gpu ? cfg.get_maximum_gpu_render_memory_size() : cfg.get_maximum_cpu_render_memory_size());
    auto vk_dev = e.get_logical_device().get_vulkan_data();
    VkMemoryAllocateInfo info;
    GX_SET_ZERO(info)
    info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    info.allocationSize = static_cast<VkDeviceSize>(aligned_size);
    info.memoryTypeIndex = type_index;
    VkDeviceMemory vulkan_data = nullptr;
    void* data = nullptr;
    GX_VK_CHK(vkAllocateMemory(vk_dev, &info, nullptr, &vulkan_data))
    if (Place::Cpu == place) {
        GX_VK_CHK(vkMapMemory(vk_dev, vulkan_data, 0, static_cast<VkDeviceSize>(aligned_size), 0, &data))
    }
    std::shared_ptr<Memory> result(new Memory(
        e, nullptr, core::Allocator::construct(aligned_size), data, place, type_index, vulkan_data));
    result->self = result;
    return result;
}

#endif
