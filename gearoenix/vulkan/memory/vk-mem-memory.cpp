#include "vk-mem-memory.hpp"
#include "../../core/cr-static.hpp"
#include "../device/vk-dev-logical.hpp"
#include "../device/vk-dev-physical.hpp"
#include "../vk-check.hpp"
#include "../vk-instance.hpp"
gearoenix::render::memory::Memory::Memory(
    device::Logical* logical_device, const VkMemoryRequirements& mem_reqs, uint32_t place)
    : logical_device(logical_device)
{
    const device::Physical* pdev = logical_device->get_physical_device();
    const Linker* l = pdev->get_instance()->get_linker();
    VkMemoryAllocateInfo mem_alloc;
    setz(mem_alloc);
    mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    mem_alloc.allocationSize = mem_reqs.size;
    mem_alloc.memoryTypeIndex = pdev->get_memory_type_index(mem_reqs.memoryTypeBits, place);
    VKC(l->vkAllocateMemory(logical_device->get_vulkan_data(), &mem_alloc, nullptr, &vulkan_data));
}
gearoenix::render::memory::Memory::~Memory()
{
    const Linker* l = logical_device->get_physical_device()->get_instance()->get_linker();
    l->vkFreeMemory(logical_device->get_vulkan_data(), vulkan_data, nullptr);
}
const gearoenix::render::device::Logical* gearoenix::render::memory::Memory::get_logical_device() const
{
    return logical_device;
}

gearoenix::render::device::Logical* gearoenix::render::memory::Memory::get_logical_device() { return logical_device; }

const VkDeviceMemory& gearoenix::render::memory::Memory::get_vulkan_data() { return vulkan_data; }
