#include "gx-vk-sync-fence.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../device/gx-vk-dev-logical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"

gearoenix::vulkan::sync::Fence::Fence(const bool signaled)
    : vulkan_data(
          device::Logical::get().get_device(),
          vk::FenceCreateInfo(signaled ? vk::FenceCreateFlagBits::eSignaled : vk::FenceCreateFlags { }))
{
}

gearoenix::vulkan::sync::Fence::~Fence() = default;

void gearoenix::vulkan::sync::Fence::wait()
{
    if (const auto result = device::Logical::get().get_device().waitForFences(*vulkan_data, vk::True, UINT64_MAX); result != vk::Result::eSuccess) {
        GX_LOG_F("Failed to wait for fence: " << vk::to_string(result));
    }
}

void gearoenix::vulkan::sync::Fence::reset()
{
    device::Logical::get().get_device().resetFences(*vulkan_data);
}

std::vector<std::shared_ptr<gearoenix::vulkan::sync::Fence>> gearoenix::vulkan::sync::Fence::create_frame_based(const bool signaled)
{
    std::vector<std::shared_ptr<Fence>> result(engine::Engine::get_frames_count());
    for (auto& s : result) {
        s = std::make_shared<Fence>(signaled);
    }
    return result;
}

#endif
