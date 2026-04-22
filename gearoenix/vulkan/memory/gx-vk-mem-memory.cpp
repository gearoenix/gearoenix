#include "gx-vk-mem-memory.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/allocator/gx-cr-alc-range.hpp"
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../../platform/gx-plt-log.hpp"
#include "../buffer/gx-vk-buf-buffer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"

gearoenix::vulkan::memory::Memory::Memory(
    std::shared_ptr<Memory> parent,
    std::shared_ptr<core::allocator::Range> allocator,
    void* const data,
    const Place place,
    const std::uint32_t type_index,
    const vk::DeviceMemory vulkan_data)
    : parent(std::move(parent))
    , allocator(std::move(allocator))
    , data(data)
    , place(place)
    , type_index(type_index)
    , vulkan_data(vulkan_data)
{
}

gearoenix::vulkan::memory::Memory::~Memory()
{
    if (!parent) {
        device::Logical::get().get_vulkan_data().freeMemory(vulkan_data);
        vulkan_data = nullptr;
    }
}

std::shared_ptr<gearoenix::vulkan::memory::Memory> gearoenix::vulkan::memory::Memory::allocate(const std::int64_t size, const std::int64_t alignment)
{
    auto alc = allocator->allocate(size, alignment);
    if (nullptr == alc) {
        GX_LOG_D("No more space left in this Vulkan memory");
        return nullptr;
    }
    void* const new_data = data == nullptr ? nullptr : reinterpret_cast<void*>(reinterpret_cast<std::intptr_t>(data) + alc->get_offset() - allocator->get_offset());
    std::shared_ptr<Memory> result(new Memory(self.lock(), std::move(alc), new_data, place, type_index, vulkan_data));
    result->self = result;
    return result;
}

std::int64_t gearoenix::vulkan::memory::Memory::get_max_gpu_needed_size(const bool is_unified_memory)
{
    const auto& cfg = render::RuntimeConfiguration::get();
    const auto buffer_req = buffer::Buffer::get_max_gpu_needed_size(is_unified_memory);
    return cfg.get_maximum_allowed_textures_memory_size() + buffer_req;
}

std::int64_t gearoenix::vulkan::memory::Memory::get_max_gpu_needed_size()
{
    return get_max_gpu_needed_size(device::Physical::get().get_unified_memory());
}

std::int64_t gearoenix::vulkan::memory::Memory::get_max_cpu_needed_size()
{
    GX_ASSERT_D(!device::Physical::get().get_unified_memory());
    return buffer::Buffer::get_max_cpu_needed_size();
}

std::shared_ptr<gearoenix::vulkan::memory::Memory> gearoenix::vulkan::memory::Memory::construct(const Place in_place, const std::uint32_t type_index)
{
    const auto place = device::Physical::get().get_unified_memory() ? Place::Gpu : in_place;
    const auto size = static_cast<vk::DeviceSize>(place == Place::Gpu ? get_max_gpu_needed_size() : get_max_cpu_needed_size());
    return construct(place, type_index, size);
}

std::shared_ptr<gearoenix::vulkan::memory::Memory> gearoenix::vulkan::memory::Memory::construct(const Place in_place, const std::uint32_t type_index, const vk::DeviceSize size)
{
    const auto place = device::Physical::get().get_unified_memory() ? Place::Gpu : in_place;
    const auto is_gpu = place == Place::Gpu;

    const auto& mem_props = device::Physical::get().get_memory_properties();
    GX_ASSERT_D(size <= mem_props.memoryHeaps[mem_props.memoryTypes[type_index].heapIndex].size);

    const auto dev = device::Logical::get().get_vulkan_data();
    const auto rtx_enabled = device::Physical::get().get_rtx_supported();

    constexpr vk::MemoryAllocateFlagsInfo flags_info(vk::MemoryAllocateFlagBits::eDeviceAddress);

    vk::MemoryAllocateInfo info(size, type_index);
    if (is_gpu && rtx_enabled) {
        info.pNext = &flags_info;
    }

    const vk::DeviceMemory vulkan_data = dev.allocateMemory(info);
    void* data = nullptr;
    // Map memory whenever it is host-visible, not just for CPU-placed allocations.
    // On unified-memory GPUs, GPU allocations are also host-visible and can be mapped
    // for direct CPU access, eliminating the need for staging buffers.
    if (mem_props.memoryTypes[type_index].propertyFlags & vk::MemoryPropertyFlagBits::eHostVisible) {
        data = dev.mapMemory(vulkan_data, 0, size, { });
        GX_ASSERT_D(nullptr != data);
    }
    std::shared_ptr<Memory> result(new Memory(nullptr, core::allocator::Range::construct(static_cast<std::int64_t>(size)), data, place, type_index, vulkan_data));
    result->self = result;
    return result;
}

#endif
