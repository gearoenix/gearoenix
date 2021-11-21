#include "gx-vk-qu-queue.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../command/gx-vk-cmd-buffer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-check.hpp"
#include "../sync/gx-vk-sync-fence.hpp"
#include "../sync/gx-vk-sync-semaphore.hpp"
#include "..\..\dxr\gx-dxr-queue.hpp"
#include "gx-vk-qu-graph.hpp"

void gearoenix::vulkan::queue::Queue::submit(
    const std::size_t wait_semaphores_count,
    const VkSemaphore* const wait_semaphores,
    const VkPipelineStageFlags* const wait_stages,
    const std::size_t commands_count,
    const VkCommandBuffer* const commands,
    const std::size_t signal_semaphores_count,
    const VkSemaphore* const signal_semaphores,
    VkFence fence) noexcept
{
    VkSubmitInfo info;
    GX_SET_ZERO(info)
    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    info.waitSemaphoreCount = static_cast<std::uint32_t>(wait_semaphores_count);
    info.pWaitSemaphores = wait_semaphores;
    info.pWaitDstStageMask = wait_stages;
    info.commandBufferCount = static_cast<std::uint32_t>(commands_count);
    info.pCommandBuffers = commands;
    info.signalSemaphoreCount = static_cast<std::uint32_t>(signal_semaphores_count);
    info.pSignalSemaphores = signal_semaphores;
    GX_VK_CHK(vkQueueSubmit(vulkan_data, 1, &info, fence))
}

void gearoenix::vulkan::queue::Queue::submit() noexcept
{
    graph->update(e.get_frame_number());
    auto& submit_data = graph->submit_data;
    auto& submit_data_front = submit_data.front();
    auto& submit_data_back = submit_data.back();
    std::get<0>(submit_data_front).push_back(graph->start_semaphore[e.get_frame_number()]->get_vulkan_data());
    std::get<1>(submit_data_front).push_back(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);
    std::get<2>(submit_data_front).clear();
    std::get<2>(submit_data_back).clear();
    std::get<3>(submit_data_back).push_back(graph->end_semaphore[e.get_frame_number()]->get_vulkan_data());

    const auto submit_data_end = submit_data.size() - 1;
    for (std::size_t submit_data_index = 0; submit_data_index < submit_data_end; ++submit_data_index) {
        const auto& [ws, ps, cs, ss] = submit_data[submit_data_index];
        submit(ws.size(), ws.data(), ps.data(), cs.size(), cs.data(), ss.size(), ss.data());
    }
    const auto& [ws, ps, cs, ss] = submit_data.back();
    submit(
        ws.size(), ws.data(), ps.data(), cs.size(), cs.data(), ss.size(), ss.data(),
        graph->fence[e.get_frame_number()]->get_vulkan_data());
}

gearoenix::vulkan::queue::Queue::Queue(engine::Engine& e) noexcept
    : e(e)
    , graph(new Graph(e))
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

void gearoenix::vulkan::queue::Queue::start_frame() noexcept
{
    auto& fence = *graph->fence[e.get_frame_number()];
    fence.wait();
    fence.reset();
}

bool gearoenix::vulkan::queue::Queue::present(
    Swapchain& swapchain,
    const std::uint32_t image_index) noexcept
{
    GX_GUARD_LOCK(this)
    submit();

    VkPresentInfoKHR info;
    GX_SET_ZERO(info)
    info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = graph->end_semaphore[e.get_frame_number()]->get_vulkan_data_ptr();
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

const gearoenix::vulkan::sync::Semaphore& gearoenix::vulkan::queue::Queue::get_present_semaphore() noexcept
{
    return *graph->start_semaphore[e.get_frame_number()];
}

std::vector<std::shared_ptr<gearoenix::vulkan::command::Buffer>> gearoenix::vulkan::queue::Queue::place_node_between(
    const std::string& previous_node_name,
    const std::string& node_name,
    const VkPipelineStageFlags wait_stage,
    const std::string& next_node_name) noexcept
{
    GX_GUARD_LOCK(this)
    auto& nodes = graph->nodes;
    auto previous_search = nodes.find(previous_node_name);
    auto next_search = nodes.find(next_node_name);

    GX_CHECK_EQUAL_D(nodes.end(), nodes.find(node_name))
    GX_CHECK_NOT_EQUAL_D(nodes.end(), previous_search)
    GX_CHECK_NOT_EQUAL_D(nodes.end(), next_search)

    auto* const previous_node = &previous_search->second;
    auto* const next_node = &next_search->second;
    auto* const node = &nodes.emplace(node_name, Node(e, node_name, wait_stage)).first->second;

    auto& previous_consumers = previous_node->consumers;
    auto& next_providers = next_node->providers;

    auto previous_old_consumer_search = previous_consumers.find(next_node_name);
    auto next_old_provider_search = next_providers.find(previous_node_name);

    GX_CHECK_NOT_EQUAL_D(previous_consumers.end(), previous_old_consumer_search)
    GX_CHECK_NOT_EQUAL_D(next_providers.end(), next_old_provider_search)

    previous_consumers.erase(previous_old_consumer_search);
    next_providers.erase(next_old_provider_search);

    Node::connect(previous_node, node, e);
    Node::connect(node, next_node, e);

    return node->cmds;
}

#endif
