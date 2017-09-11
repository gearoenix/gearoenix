#include "vk-sync-fence.hpp"
#include "../../core/cr-build-configuration.hpp"
#include "../../core/cr-static.hpp"
#include "../device/vk-dev-logical.hpp"
#include "../device/vk-dev-physical.hpp"
#include "../image/vk-img-image.hpp"
#include "../image/vk-img-view.hpp"
#include "../vk-check.hpp"
#include "../vk-instance.hpp"
#include "../vk-render-pass.hpp"

gearoenix::render::sync::Fence::Fence(
    const std::shared_ptr<device::Logical>& logical_device, bool signaled)
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
