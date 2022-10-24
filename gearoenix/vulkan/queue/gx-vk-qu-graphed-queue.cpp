#include "gx-vk-qu-graphed-queue.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../command/gx-vk-cmd-buffer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-check.hpp"
#include "../sync/gx-vk-sync-fence.hpp"
#include "../sync/gx-vk-sync-semaphore.hpp"

void gearoenix::vulkan::queue::GraphedQueue::submit() noexcept
{
    const auto frame_index = qu.get_e().get_frame_number();
    // graph.update(); // TODO: move this to the function that actually updates the graph, do not always do it.
    const auto& submit_data = graph.get_submit_data();
    const auto submit_data_end = submit_data.size() - 1;
    for (std::size_t submit_data_index = 0; submit_data_index < submit_data_end; ++submit_data_index) {
        const auto& sd = submit_data[submit_data_index];
        const auto& frame = sd.frame_data[frame_index];
        qu.submit(
            frame.waits.size(), &(*frame.waits.begin()),
            &sd.pipeline_stage,
            1, &frame.cmd,
            frame.signals.size(), &(*frame.signals.begin()));
    }
    const auto& sd = submit_data.back();
    const auto& frame = sd.frame_data[frame_index];
    qu.submit(
        frame.waits.size(), &(*frame.waits.begin()),
        &sd.pipeline_stage,
        1, &frame.cmd,
        frame.signals.size(), &(*frame.signals.begin()),
        graph.fence[frame_index]->get_vulkan_data());
}

gearoenix::vulkan::queue::GraphedQueue::GraphedQueue(engine::Engine& e) noexcept
    : graph(e)
    , qu(e)
{
}

gearoenix::vulkan::queue::GraphedQueue::~GraphedQueue() noexcept = default;

void gearoenix::vulkan::queue::GraphedQueue::start_frame() noexcept
{
    auto& fence = *graph.fence[qu.get_e().get_frame_number()];
    fence.wait();
    fence.reset();
}

bool gearoenix::vulkan::queue::GraphedQueue::present(
    Swapchain& swapchain,
    const std::uint32_t image_index) noexcept
{
    submit();

    VkPresentInfoKHR info;
    GX_SET_ZERO(info);
    info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = graph.end_semaphore[qu.get_e().get_frame_number()]->get_vulkan_data_ptr();
    info.swapchainCount = 1;
    info.pSwapchains = swapchain.get_vulkan_data_ptr();
    info.pImageIndices = &image_index;
    const auto present_result = vkQueuePresentKHR(qu.get_vulkan_data(), &info);
    if (VK_ERROR_OUT_OF_DATE_KHR == present_result) {
        return false;
    } else if (VK_SUCCESS != present_result) {
        GX_LOG_F("Presentation failed with result: " << result_to_string(present_result));
    }
    return true;
}

const gearoenix::vulkan::sync::Semaphore& gearoenix::vulkan::queue::GraphedQueue::get_present_semaphore() noexcept
{
    return *graph.start_semaphore[qu.get_e().get_frame_number()];
}

#endif