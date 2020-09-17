#define VMA_IMPLEMENTATION
#include "gx-vk-mem-manager.hpp"
#ifdef GX_USE_VULKAN
#include "../../system/gx-sys-log.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../gx-vk-check.hpp"
#include "../gx-vk-instance.hpp"
#include "../gx-vk-surface.hpp"

gearoenix::vulkan::memory::Manager::Manager(std::shared_ptr<device::Logical> ld) noexcept
    : logical_device(std::move(ld))
{
    const auto& phy_dev = logical_device->get_physical_device();
    VmaAllocatorCreateInfo allocator_info {};
    allocator_info.physicalDevice = phy_dev->get_vulkan_data();
    allocator_info.device = logical_device->get_vulkan_data();
    allocator_info.instance = phy_dev->get_surface()->get_instance()->get_vulkan_data();
    const auto result = vmaCreateAllocator(&allocator_info, &allocator);
    if (VK_SUCCESS != result) {
        GXLOGF("Error in initializing memory allocator, result: " << result_to_string(result))
    }
}

gearoenix::vulkan::memory::Manager::~Manager() noexcept
{
    vmaDestroyAllocator(allocator);
}
#endif
