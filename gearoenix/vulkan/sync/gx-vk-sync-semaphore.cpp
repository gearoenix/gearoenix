#include "gx-vk-sync-semaphore.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../device/gx-vk-dev-logical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-marker.hpp"

gearoenix::vulkan::sync::Semaphore::Semaphore([[maybe_unused]] const std::string& name)
    : vulkan_data(
          device::Logical::get().get_device(),
          vk::SemaphoreCreateInfo())
{
    GX_VK_MARK(name, *vulkan_data);
}

gearoenix::vulkan::sync::Semaphore::~Semaphore() = default;

std::vector<std::shared_ptr<gearoenix::vulkan::sync::Semaphore>> gearoenix::vulkan::sync::Semaphore::create_frame_based(const std::string& base_name)
{
    std::vector<std::shared_ptr<Semaphore>> result(core::Singleton<engine::Engine>::get().get_frames_count());
    int frame_index = 0;
    for (auto& s : result) {
        s = std::make_shared<Semaphore>(base_name + "_frame_" + std::to_string(++frame_index));
    }
    return result;
}

#endif
