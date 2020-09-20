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
    //allocator_info.pVulkanFunctions.
    GX_VK_CHK(vmaCreateAllocator(&allocator_info, &allocator))
}

gearoenix::vulkan::memory::Manager::~Manager() noexcept
{
    vmaDestroyAllocator(allocator);
}

std::tuple<VkImage, std::shared_ptr<gearoenix::vulkan::memory::Memory>> gearoenix::vulkan::memory::Manager::create_image(const VkImageCreateInfo& info) noexcept
{
    VmaAllocationCreateInfo alloc_info {
        .usage = VMA_MEMORY_USAGE_GPU_ONLY
    };
    VkImage img;
    VmaAllocation alc;
    GX_VK_CHK(vmaCreateImage(allocator, &info, &alloc_info, &img, &alc, nullptr))
    return std::make_tuple(img, std::make_shared<Memory>(shared_from_this(), alc));
}

void gearoenix::vulkan::memory::Manager::destroy_image(VkImage image, std::shared_ptr<Memory>& mem) noexcept
{
    vmaDestroyImage(allocator, image, mem->get_allocation());
    mem->set_not_deleted(false);
}

#endif
