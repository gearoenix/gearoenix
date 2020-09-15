#include "gx-vk-sync-fence.hpp"
#ifdef USE_VULKAN
#include "../../core/gx-cr-build-configuration.hpp"
#include "../../core/gx-cr-static.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../gx-vk-check.hpp"
#include "../gx-vk-instance.hpp"
#include "../gx-vk-render-pass.hpp"
#include "../image/gx-vk-img-image.hpp"
#include "../image/gx-vk-img-view.hpp"

gearoenix::render::sync::Fence::Fence(device::Logical* logical_device, bool signaled)
    : logical_device(logical_device)
{
    VkFenceCreateInfo fence_create_info;
    setz(fence_create_info);
    fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    if (signaled) {
        fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    }
    VKC(logical_device->get_physical_device()
            ->get_instance()
            ->get_linker()
            ->vkCreateFence(logical_device->get_vulkan_data(), &fence_create_info,
                nullptr, &vulkan_data));
}

gearoenix::render::sync::Fence::~Fence()
{
    logical_device->get_physical_device()
        ->get_instance()
        ->get_linker()
        ->vkDestroyFence(logical_device->get_vulkan_data(), vulkan_data, nullptr);
}

void gearoenix::render::sync::Fence::wait()
{
    VKC(logical_device->get_physical_device()
            ->get_instance()
            ->get_linker()
            ->vkWaitForFences(logical_device->get_vulkan_data(), 1, &vulkan_data,
                VK_TRUE, UINT64_MAX));
}

const VkFence& gearoenix::render::sync::Fence::get_vulkan_data() const
{
    return vulkan_data;
}
#endif
