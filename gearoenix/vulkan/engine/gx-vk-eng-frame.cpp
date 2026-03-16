#include "gx-vk-eng-frame.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../command/gx-vk-cmd-buffer.hpp"
#include "../command/gx-vk-cmd-manager.hpp"
#include "../sync/gx-vk-sync-fence.hpp"
#include "../sync/gx-vk-sync-semaphore.hpp"

gearoenix::vulkan::engine::Frame::Frame(const int frame_index)
    : render_fence(new sync::Fence(true))
    , present_semaphore(new sync::Semaphore("frame-preset-" + std::to_string(frame_index)))
    , end_semaphore(new sync::Semaphore("frame-end" + std::to_string(frame_index)))
    , cmd(command::Manager::get().create())
{
}

gearoenix::vulkan::engine::Frame::~Frame() = default;

void gearoenix::vulkan::engine::Frame::start()
{
    render_fence->wait();
    render_fence->reset();
}

#endif