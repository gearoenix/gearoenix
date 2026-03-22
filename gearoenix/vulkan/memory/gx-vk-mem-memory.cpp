#include "gx-vk-mem-memory.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/allocator/gx-cr-alc-range.hpp"
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../../math/gx-math-numeric.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../../platform/gx-plt-log.hpp"
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

std::int64_t gearoenix::vulkan::memory::Memory::align(const std::int64_t sz)
{
    return math::Numeric::align(sz, static_cast<std::int64_t>(device::Physical::get().get_max_memory_alignment()));
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

std::shared_ptr<gearoenix::vulkan::memory::Memory> gearoenix::vulkan::memory::Memory::construct(const Place place, const std::uint32_t type_index)
{
    const auto is_gpu = place == Place::Gpu;
    const auto& cfg = render::RuntimeConfiguration::get();
    const auto aligned_size = align(is_gpu ? cfg.get_maximum_gpu_render_memory_size() : cfg.get_maximum_cpu_render_memory_size());
    auto dev = device::Logical::get().get_vulkan_data();
    const auto rtx_enabled = device::Physical::get().get_rtx_supported();

    vk::MemoryAllocateFlagsInfo flags_info(vk::MemoryAllocateFlagBits::eDeviceAddress);

    vk::MemoryAllocateInfo info(
        static_cast<vk::DeviceSize>(aligned_size),
        type_index);
    if (is_gpu && rtx_enabled) {
        info.pNext = &flags_info;
    }

    const vk::DeviceMemory vulkan_data = dev.allocateMemory(info);
    void* data = nullptr;
    if (Place::Cpu == place) {
        data = dev.mapMemory(vulkan_data, 0, static_cast<vk::DeviceSize>(aligned_size), { });
        GX_ASSERT_D(nullptr != data);
    }
    std::shared_ptr<Memory> result(new Memory(nullptr, core::allocator::Range::construct(aligned_size), data, place, type_index, vulkan_data));
    result->self = result;
    return result;
}

#endif
