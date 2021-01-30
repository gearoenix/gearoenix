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
    Manager* const manager,
    Memory* const parent,
    core::Allocator&& allocator,
    void* const data,
    const Place place,
    const std::uint32_t type_index,
    VkDeviceMemory vulkan_data) noexcept
    : manager(manager)
    , parent(parent)
    , allocator(std::move(allocator))
    , data(data)
    , place(place)
    , type_index(type_index)
    , vulkan_data(vulkan_data)
{
}

gearoenix::vulkan::memory::Memory::Memory(Memory&& o) noexcept
    : manager(o.manager)
    , parent(o.parent)
    , allocator(std::move(o.allocator))
    , data(o.data)
    , place(o.place)
    , type_index(o.type_index)
    , vulkan_data(o.vulkan_data)
{
    o.vulkan_data = nullptr;
}

gearoenix::vulkan::memory::Memory::~Memory() noexcept
{
    if (parent == nullptr && vulkan_data != nullptr) {
        vkFreeMemory(manager->get_logical_device().get_vulkan_data(), vulkan_data, nullptr);
        vulkan_data = nullptr;
    }
}

gearoenix::vulkan::memory::Memory& gearoenix::vulkan::memory::Memory::operator=(Memory&& o) noexcept
{
    manager = o.manager;
    parent = o.parent;
    allocator = std::move(o.allocator);
    data = o.data;
    place = o.place;
    type_index = o.type_index;
    vulkan_data = o.vulkan_data;
    return *this;
}

std::optional<gearoenix::vulkan::memory::Memory> gearoenix::vulkan::memory::Memory::allocate(const std::size_t size) noexcept
{
    const auto aligned_size = manager->align(size);
    auto alc = allocator.allocate(aligned_size);
    if (!alc.has_value()) {
        GX_LOG_D("No more space left in this Vulkan memory")
        return std::nullopt;
    }
    void* const new_data = reinterpret_cast<void*>(reinterpret_cast<std::size_t>(data) + alc->get_offset());
    return Memory(manager, this, std::move(alc.value()), new_data, place, type_index, vulkan_data);
}

#endif
