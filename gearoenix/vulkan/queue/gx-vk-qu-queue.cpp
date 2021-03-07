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

void gearoenix::vulkan::queue::Queue::Node::clear_traversing_level() noexcept
{
    traversal_level = -1;
    traversed = false;
    for (auto& c : consumers)
        c.second.first->clear_traversing_level();
}

void gearoenix::vulkan::queue::Queue::Node::update_traversing_level(const int tl) noexcept
{
    if (tl > traversal_level) {
        traversal_level = tl;
        for (auto& c : consumers)
            c.second.first->update_traversing_level(tl + 1);
    }
}

void gearoenix::vulkan::queue::Queue::Node::connect(
    Node* const provider,
    Node* const consumer,
    engine::Engine& e) noexcept
{
    GX_CHECK_EQUAL_D(consumer->providers.end(), consumer->providers.find(provider->name))
    GX_CHECK_EQUAL_D(provider->consumers.end(), provider->consumers.find(consumer->name))

    auto semaphores = create_frame_semaphores(e);

    consumer->providers.emplace(provider->name, std::make_pair(provider, semaphores));
    provider->consumers.emplace(consumer->name, std::make_pair(consumer, std::move(semaphores)));
}

gearoenix::vulkan::queue::Queue::Graph::Graph(engine::Engine& e) noexcept
    : start_semaphore(create_frame_semaphores(e))
    , nodes {
        { START_FRAME, Node(e, START_FRAME, 0) },
        { END_FRAME, Node(e, END_FRAME, 0) }
    }
    , start(&nodes.find(START_FRAME)->second)
    , end(&nodes.find(END_FRAME)->second)
{
    Node::connect(start, end, e);
}

gearoenix::vulkan::queue::Queue::Graph::~Graph() noexcept = default;

void gearoenix::vulkan::queue::Queue::Graph::update_traversing_level() noexcept
{
    start->clear_traversing_level();
    start->update_traversing_level(0);
}

void gearoenix::vulkan::queue::Queue::Graph::clear_submit_data() noexcept
{
    for (auto& ps_map : submit_data) {
        for (auto& [ps, data] : ps_map) {
            (void)ps;
            auto& [ws, cs, ss] = data;
            ws.clear();
            cs.clear();
            ss.clear();
        }
    }
}

void gearoenix::vulkan::queue::Queue::Graph::update_submit_data(Node& n, const std::size_t frame_number) noexcept
{
    if (n.traversed)
        return;
    n.traversed = true;
    while (static_cast<std::size_t>(n.traversal_level) + 1 > submit_data.size())
        submit_data.emplace_back();
    auto& [ws, cs, ss] = submit_data[n.traversal_level][n.wait_stage];
    for (auto& p : n.providers) {
        ws.emplace_back(p.second.second[frame_number]->get_vulkan_data());
    }
    cs.emplace_back(n.cmds[frame_number]->get_vulkan_data());
    for (auto& c : n.consumers) {
        ws.emplace_back(c.second.second[frame_number]->get_vulkan_data());
    }
}

void gearoenix::vulkan::queue::Queue::Graph::update(const std::size_t frame_number) noexcept
{
    update_traversing_level();
    clear_submit_data();
    update_submit_data(*start, frame_number);
}

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

void gearoenix::vulkan::queue::Queue::submit() noexcept
{
    graph.update(e.get_frame_number());
    const auto& submit_data = graph.submit_data;
    const auto& start_data = submit_data[0].find(0)->second;
    submit(
        1, graph.start_semaphore[e.get_frame_number()]->get_vulkan_data_ptr(),
        0, nullptr,
        std::get<2>(start_data).size(), std::get<2>(start_data).data(),
        0);
    const auto sd_size = submit_data.size() - 1;
    for (std::size_t i = 1; i < sd_size; ++i) {
        const auto& sd = submit_data[i];
        for (const auto& [wait_stage, wcs] : sd) {
            const auto& [ws, cs, ss] = wcs;
            submit(ws.size(), ws.data(), cs.size(), cs.data(), ss.size(), ss.data(), wait_stage);
        }
    }
}

void gearoenix::vulkan::queue::Queue::submit(
    const std::size_t wait_semaphores_count,
    const VkSemaphore* const wait_semaphores,
    const std::size_t commands_count,
    const VkCommandBuffer* const commands,
    const std::size_t signal_semaphores_count,
    const VkSemaphore* const signal_semaphores,
    const VkPipelineStageFlags wait_stage) noexcept
{
    VkSubmitInfo info;
    GX_SET_ZERO(info)
    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    info.waitSemaphoreCount = static_cast<std::uint32_t>(wait_semaphores_count);
    info.pWaitSemaphores = wait_semaphores;
    info.pWaitDstStageMask = &wait_stage;
    info.commandBufferCount = static_cast<std::uint32_t>(commands_count);
    info.pCommandBuffers = commands;
    info.signalSemaphoreCount = static_cast<std::uint32_t>(signal_semaphores_count);
    info.pSignalSemaphores = signal_semaphores;
    GX_GUARD_LOCK(this)
    GX_VK_CHK(vkQueueSubmit(vulkan_data, 1, &info, nullptr))
}

#endif
