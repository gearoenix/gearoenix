#include "gx-vk-eng-frame.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../gx-vk-swapchain.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../sync/gx-vk-sync-semaphore.hpp"
#include "../sync/gx-vk-sync-fence.hpp"
#include "../command/gx-vk-cmd-buffer.hpp"
#include "../command/gx-vk-cmd-manager.hpp"


gearoenix::vulkan::engine::Frame::Frame(std::shared_ptr<image::View>&& view)
    : view(std::move(view))
    , render_fence(new sync::Fence(true))
    , present_semaphore(new sync::Semaphore())
    , end_semaphore(new sync::Semaphore())
    , cmd(command::Manager::get().create())
{
}

gearoenix::vulkan::engine::Frame::~Frame() = default;

#endif