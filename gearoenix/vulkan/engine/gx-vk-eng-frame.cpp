#include "gx-vk-eng-frame.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../command/gx-vk-cmd-buffer.hpp"
#include "../command/gx-vk-cmd-manager.hpp"
#include "../gx-vk-swapchain.hpp"
#include "../sync/gx-vk-sync-fence.hpp"
#include "../sync/gx-vk-sync-semaphore.hpp"

gearoenix::vulkan::engine::Frame::Frame(std::shared_ptr<image::View>&& view, std::shared_ptr<image::View>&& imgui_view, const int frame_index)
    : view(std::move(view))
    , imgui_view(std::move(imgui_view))
    , render_fence(new sync::Fence(true))
    , present_semaphore(new sync::Semaphore("frame-preset-" + std::to_string(frame_index)))
    , end_semaphore(new sync::Semaphore("frame-end" + std::to_string(frame_index)))
    , cmd(command::Manager::get().create())
{
}

gearoenix::vulkan::engine::Frame::~Frame() = default;

#endif