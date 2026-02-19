#include "gx-vk-mem-memory.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/allocator/gx-cr-alc-range.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../../math/gx-math-numeric.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-check.hpp"

gearoenix::vulkan::memory::Memory::Memory(std::shared_ptr<Memory> parent, std::shared_ptr<core::allocator::Range> allocator, void* const data, const Place place, const std::uint32_t type_index, const VkDeviceMemory vulkan_data)
    : parent(std::move(parent))
    , allocator(std::move(allocator))
    , data(data)
    , place(place)
    , type_index(type_index)
    , vulkan_data(vulkan_data)
{
}

std::int64_t gearoenix::vulkan::memory::Memory::align(const std::int64_t sz) { return math::Numeric::align(sz, static_cast<std::int64_t>(device::Physical::get().get_max_memory_alignment())); }

gearoenix::vulkan::memory::Memory::~Memory()
{
    if (parent == nullptr) {
        vkFreeMemory(device::Logical::get().get_vulkan_data(), vulkan_data, nullptr);
        vulkan_data = nullptr;
    }
}

std::shared_ptr<gearoenix::vulkan::memory::Memory> gearoenix::vulkan::memory::Memory::allocate(const std::int64_t size)
{
    const auto aligned_size = align(size);
    auto alc = allocator->allocate(aligned_size);
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
    auto vk_dev = device::Logical::get().get_vulkan_data();
    const auto rtx_enabled = device::Physical::get().get_rtx_supported();

    VkMemoryAllocateFlagsInfo mem_alloc_info;
    GX_SET_ZERO(mem_alloc_info);
    mem_alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
    mem_alloc_info.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;

    VkMemoryAllocateInfo info;
    GX_SET_ZERO(info);
    info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    info.allocationSize = static_cast<VkDeviceSize>(aligned_size);
    info.memoryTypeIndex = type_index;
    info.pNext = is_gpu && rtx_enabled ? &mem_alloc_info : nullptr;

    VkDeviceMemory vulkan_data = nullptr;
    void* data = nullptr;
    GX_VK_CHK(vkAllocateMemory(vk_dev, &info, nullptr, &vulkan_data));
    if (Place::Cpu == place) {
        GX_VK_CHK(vkMapMemory(vk_dev, vulkan_data, 0, static_cast<VkDeviceSize>(aligned_size), 0, &data));
        GX_ASSERT_D(nullptr != data);
    }
    std::shared_ptr<Memory> result(new Memory(nullptr, core::allocator::Range::construct(aligned_size), data, place, type_index, vulkan_data));
    result->self = result;
    return result;
}

#endif
