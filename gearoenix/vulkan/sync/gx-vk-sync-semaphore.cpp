#include "gx-vk-sync-semaphore.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-check.hpp"

gearoenix::vulkan::sync::Semaphore::Semaphore()
{
    VkSemaphoreCreateInfo info;
    GX_SET_ZERO(info);
    info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    GX_VK_CHK(vkCreateSemaphore(device::Logical::get().get_vulkan_data(), &info, nullptr, &vulkan_data));
}

gearoenix::vulkan::sync::Semaphore::~Semaphore()
{
    if (nullptr != vulkan_data) {
        vkDestroySemaphore(device::Logical::get().get_vulkan_data(), vulkan_data, nullptr);
        vulkan_data = nullptr;
    }
}

const VkSemaphore* gearoenix::vulkan::sync::Semaphore::get_vulkan_data_ptr() const
{
    return &vulkan_data;
}

std::vector<std::shared_ptr<gearoenix::vulkan::sync::Semaphore>> gearoenix::vulkan::sync::Semaphore::create_frame_based()
{
    std::vector<std::shared_ptr<Semaphore>> result(core::Singleton<engine::Engine>::get().get_frames_count());
    for (auto& s : result) {
        s = std::make_shared<Semaphore>();
    }
    return result;
}

#endif