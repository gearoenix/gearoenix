#include "gx-vk-qu-queue.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../command/gx-vk-cmd-buffer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-check.hpp"
#include "../gx-vk-swapchain.hpp"
#include "../sync/gx-vk-sync-fence.hpp"
#include "../sync/gx-vk-sync-semaphore.hpp"

[[nodiscard]] static std::vector<std::shared_ptr<gearoenix::vulkan::sync::Semaphore>> create_frame_semaphores(
    const gearoenix::vulkan::engine::Engine& e) noexcept
{
    std::vector<std::shared_ptr<gearoenix::vulkan::sync::Semaphore>> result(e.get_frames_count());
    const auto& d = e.get_logical_device();
    for (auto& s : result)
        s = std::make_shared<gearoenix::vulkan::sync::Semaphore>(d);
    return result;
}

[[nodiscard]] static std::vector<std::shared_ptr<gearoenix::vulkan::command::Buffer>> create_frame_cmds(
    gearoenix::vulkan::engine::Engine& e) noexcept
{
    std::vector<std::shared_ptr<gearoenix::vulkan::command::Buffer>> result(e.get_frames_count());
    auto& cmd_mgr = e.get_command_manager();
    for (auto& c : result)
        c = std::make_shared<gearoenix::vulkan::command::Buffer>(std::move(cmd_mgr.create(
            gearoenix::vulkan::command::Type::Primary)));
    return result;
}

gearoenix::vulkan::queue::Queue::Node::Node(engine::Engine& e, std::string name, VkPipelineStageFlags wait_stage) noexcept
    : name(std::move(name))
    , wait_stage(wait_stage)
    , cmds(create_frame_cmds(e))
{
}

gearoenix::vulkan::queue::Queue::Node::~Node() noexcept = default;

void gearoenix::vulkan::queue::Queue::Node::connect(
    Node* const provider,
    Node* const consumer,
    engine::Engine& e) noexcept
{
    GX_CHECK_EQUAL_D(consumer->providers.end(), consumer->providers.find(provider->name))
    GX_CHECK_EQUAL_D(provider->consumers.end(), provider->consumers.find(consumer->name))

    consumer->providers.emplace(provider->name, provider);
    provider->consumers.emplace(consumer->name, std::make_pair(consumer, create_frame_semaphores(e)));
}

gearoenix::vulkan::queue::Queue::Graph::Graph(engine::Engine& e) noexcept
    : present(create_frame_semaphores(e))
    , nodes {
        { START_FRAME, Node(e, START_FRAME, 0) },
        { END_FRAME, Node(e, END_FRAME, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT) }
    }
    , start(&nodes.find(START_FRAME)->second)
    , end(&nodes.find(END_FRAME)->second)
{
    Node::connect(start, end, e);
}

gearoenix::vulkan::queue::Queue::Graph::~Graph() noexcept = default;

gearoenix::vulkan::queue::Queue::Queue(engine::Engine& e) noexcept
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
