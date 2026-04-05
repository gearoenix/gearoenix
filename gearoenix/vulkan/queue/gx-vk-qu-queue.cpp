#include "gx-vk-qu-queue.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/gx-cr-profiler.hpp"
#include "../command/gx-vk-cmd-buffer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../sync/gx-vk-sync-fence.hpp"

#include <mutex>

gearoenix::vulkan::queue::Queue::Queue()
    : Singleton(this)
    , vulkan_data(device::Logical::get().get_vulkan_data().getQueue(device::Physical::get().get_graphics_queue_node_index(), 0))
{
}

gearoenix::vulkan::queue::Queue::~Queue() = default;

void gearoenix::vulkan::queue::Queue::submit(const command::Buffer& cmd, const sync::Fence& fence)
{
    const auto vk_cmd = cmd.get_vulkan_data();
    vk::SubmitInfo info;
    info.setCommandBuffers(vk_cmd);
    const std::lock_guard _(submission_lock);
    vulkan_data.submit(info, fence.get_vulkan_data());
}

void gearoenix::vulkan::queue::Queue::submit(const vk::ArrayProxy<const vk::Semaphore> wait_semaphores, const vk::ArrayProxy<const vk::PipelineStageFlags> wait_stages,
    const vk::ArrayProxy<const vk::CommandBuffer> commands, const vk::ArrayProxy<const vk::Semaphore> signal_semaphores, const vk::Fence fence)
{
    vk::SubmitInfo info;
    info.setWaitSemaphores(wait_semaphores);
    info.setWaitDstStageMask(wait_stages);
    info.setCommandBuffers(commands);
    info.setSignalSemaphores(signal_semaphores);
    const std::lock_guard _(submission_lock);
    GX_PROFILE_EXP(vulkan_data.submit(info, fence));
}

void gearoenix::vulkan::queue::Queue::present(const vk::PresentInfoKHR& info)
{
    const std::lock_guard _(submission_lock);
    [[maybe_unused]] const auto result = vulkan_data.presentKHR(info);
    GX_ASSERT_D(vk::Result::eSuccess == result || vk::Result::eSuboptimalKHR == result);
}

#endif
