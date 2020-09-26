#define GX_VMA_IMPL
#include "gx-vk-mem-manager.hpp"
#ifdef GX_USE_VULKAN
#include "../../system/gx-sys-log.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../gx-vk-check.hpp"
#include "../gx-vk-instance.hpp"
#include "../gx-vk-surface.hpp"
#include "gx-vk-mem-memory.hpp"

gearoenix::vulkan::memory::Manager::Manager(std::shared_ptr<device::Logical> ld) noexcept
    : logical_device(std::move(ld))
{
    const auto& phy_dev = logical_device->get_physical_device();
    VmaAllocatorCreateInfo allocator_info {};
    allocator_info.physicalDevice = phy_dev->get_vulkan_data();
    allocator_info.device = logical_device->get_vulkan_data();
    allocator_info.instance = phy_dev->get_surface()->get_instance()->get_vulkan_data();
    static const VmaVulkanFunctions pointers {
        .vkGetPhysicalDeviceProperties = Loader::vkGetPhysicalDeviceProperties,
        .vkGetPhysicalDeviceMemoryProperties = Loader::vkGetPhysicalDeviceMemoryProperties,
        .vkAllocateMemory = Loader::vkAllocateMemory,
        .vkFreeMemory = Loader::vkFreeMemory,
        .vkMapMemory = Loader::vkMapMemory,
        .vkUnmapMemory = Loader::vkUnmapMemory,
        .vkFlushMappedMemoryRanges = Loader::vkFlushMappedMemoryRanges,
        .vkInvalidateMappedMemoryRanges = Loader::vkInvalidateMappedMemoryRanges,
        .vkBindBufferMemory = Loader::vkBindBufferMemory,
        .vkBindImageMemory = Loader::vkBindImageMemory,
        .vkGetBufferMemoryRequirements = Loader::vkGetBufferMemoryRequirements,
        .vkGetImageMemoryRequirements = Loader::vkGetImageMemoryRequirements,
        .vkCreateBuffer = Loader::vkCreateBuffer,
        .vkDestroyBuffer = Loader::vkDestroyBuffer,
        .vkCreateImage = Loader::vkCreateImage,
        .vkDestroyImage = Loader::vkDestroyImage,
        .vkCmdCopyBuffer = Loader::vkCmdCopyBuffer,
    };
    allocator_info.pVulkanFunctions = &pointers;
    GX_VK_CHK(vmaCreateAllocator(&allocator_info, &allocator))
}

std::shared_ptr<gearoenix::vulkan::memory::Manager> gearoenix::vulkan::memory::Manager::construct(
    std::shared_ptr<device::Logical> logical_device) noexcept
{
    std::shared_ptr<Manager> mgr(new Manager(std::move(logical_device)));
    mgr->self = mgr;
    return mgr;
}

gearoenix::vulkan::memory::Manager::~Manager() noexcept
{
    vmaDestroyAllocator(allocator);
}

std::tuple<VkImage, std::shared_ptr<gearoenix::vulkan::memory::Memory>> gearoenix::vulkan::memory::Manager::create(
    const VkImageCreateInfo& info) noexcept
{
    const VmaAllocationCreateInfo alloc_info {
        .usage = VMA_MEMORY_USAGE_GPU_ONLY
    };
    VkImage img = nullptr;
    VmaAllocation alc = nullptr;
    VmaAllocationInfo mem_info {};
    GX_VK_CHK(vmaCreateImage(allocator, &info, &alloc_info, &img, &alc, &mem_info))
    return std::make_tuple(img, std::make_shared<Memory>(self.lock(), alc, mem_info));
}

void gearoenix::vulkan::memory::Manager::destroy(VkImage image, std::shared_ptr<Memory>& mem) noexcept
{
    vmaDestroyImage(allocator, image, mem->get_allocation());
    mem->set_not_deleted(false);
}

std::tuple<VkBuffer, std::shared_ptr<gearoenix::vulkan::memory::Memory>, void*> gearoenix::vulkan::memory::Manager::create(
    const VkBufferCreateInfo& info, const Usage usage) noexcept
{
    const VmaAllocationCreateInfo alloc_info {
        .flags = (usage == Usage::CpuToGpu || usage == Usage::Cpu) ? VMA_ALLOCATION_CREATE_MAPPED_BIT : static_cast<VmaAllocationCreateFlags>(0),
        .usage = convert(usage),
    };
    VkBuffer buffer;
    VmaAllocation allocation;
    VmaAllocationInfo allocation_info;
    vmaCreateBuffer(allocator, &info, &alloc_info, &buffer, &allocation, &allocation_info);
    return std::make_tuple(
        buffer,
        std::make_shared<Memory>(self.lock(), allocation, allocation_info),
        allocation_info.pMappedData);
}

void gearoenix::vulkan::memory::Manager::destroy(VkBuffer buff, std::shared_ptr<Memory>& mem) noexcept
{
    vmaDestroyBuffer(allocator, buff, mem->get_allocation());
    mem->set_not_deleted(false);
}

VmaMemoryUsage gearoenix::vulkan::memory::Manager::convert(const Usage usage) noexcept
{
    switch (usage) {
    case Usage::Unknown:
        return VMA_MEMORY_USAGE_UNKNOWN;
    case Usage::Cpu:
        return VMA_MEMORY_USAGE_CPU_ONLY;
    case Usage::CpuCopy:
        return VMA_MEMORY_USAGE_CPU_COPY;
    case Usage::CpuToGpu:
        return VMA_MEMORY_USAGE_CPU_TO_GPU;
    case Usage::Gpu:
        return VMA_MEMORY_USAGE_GPU_ONLY;
    case Usage::GpuToCpu:
        return VMA_MEMORY_USAGE_GPU_TO_CPU;
    }
    GX_UNEXPECTED
}

#endif
