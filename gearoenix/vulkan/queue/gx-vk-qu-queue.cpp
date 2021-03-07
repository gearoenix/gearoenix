#include "gx-vk-qu-queue.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../command/gx-vk-cmd-buffer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-check.hpp"
#include "../gx-vk-swapchain.hpp"
#include "../sync/gx-vk-sync-fence.hpp"
#include "../sync/gx-vk-sync-semaphore.hpp"

gearoenix::vulkan::queue::Queue::Node::Node(const engine::Engine&) noexcept
{
}

gearoenix::vulkan::queue::Queue::Node::~Node() noexcept = default;

gearoenix::vulkan::queue::Queue::Graph::Graph(const engine::Engine&) noexcept
{
}

gearoenix::vulkan::queue::Queue::Graph::~Graph() noexcept = default;

gearoenix::vulkan::queue::Queue::Queue(const engine::Engine& e) noexcept
    : e(e)
    , graph(e)
{
    vkGetDeviceQueue(
        e.get_logical_device().get_vulkan_data(),
        e.get_physical_device().get_graphics_queue_node_index(),
        0, &vulkan_data);
}

gearoenix::vulkan::queue::Queue::~Queue() noexcept = default;

void gearoenix::vulkan::queue::Queue::submit(command::Buffer& cmd, sync::Fence& fence) noexcept
{
    VkSubmitInfo info;
    GX_SET_ZERO(info)
    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    info.commandBufferCount = 1;
    info.pCommandBuffers = cmd.get_vulkan_data_ptr();
    GX_GUARD_LOCK(this)
    GX_VK_CHK(vkQueueSubmit(vulkan_data, 1, &info, fence.get_vulkan_data()))
}

void gearoenix::vulkan::queue::Queue::submit(
    sync::Semaphore& wait,
    command::Buffer& cmd,
    sync::Semaphore& signal,
    sync::Fence& fence,
    const VkPipelineStageFlags wait_stage) noexcept
{
    VkSubmitInfo info;
    GX_SET_ZERO(info)
    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = wait.get_vulkan_data_ptr();
    info.pWaitDstStageMask = &wait_stage;
    info.commandBufferCount = 1;
    info.pCommandBuffers = cmd.get_vulkan_data_ptr();
    info.signalSemaphoreCount = 1;
    info.pSignalSemaphores = signal.get_vulkan_data_ptr();
    GX_GUARD_LOCK(this)
    GX_VK_CHK(vkQueueSubmit(vulkan_data, 1, &info, fence.get_vulkan_data()))
}

bool gearoenix::vulkan::queue::Queue::present(
    sync::Semaphore& wait,
    Swapchain& swapchain,
    const std::uint32_t image_index) noexcept
{
    VkPresentInfoKHR info;
    GX_SET_ZERO(info)
    info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = wait.get_vulkan_data_ptr();
    info.swapchainCount = 1;
    info.pSwapchains = swapchain.get_vulkan_data_ptr();
    info.pImageIndices = &image_index;
    const auto present_result = vkQueuePresentKHR(vulkan_data, &info);
    if (VK_ERROR_OUT_OF_DATE_KHR == present_result) {
        return false;
    } else if (VK_SUCCESS != present_result) {
        GX_LOG_F("Presentation failed with result: " << result_to_string(present_result))
    }
    return true;
}

#endif
