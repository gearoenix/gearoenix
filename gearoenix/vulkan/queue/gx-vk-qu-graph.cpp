#include "gx-vk-qu-graph.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../command/gx-vk-cmd-buffer.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-check.hpp"
#include "../sync/gx-vk-sync-fence.hpp"
#include "../sync/gx-vk-sync-semaphore.hpp"
#include <algorithm>

void gearoenix::vulkan::queue::Graph::SubmitData::FrameData::clear() noexcept
{
    providers_waits.clear();
    pipeline_stages.clear();
    cmd = nullptr;
    consumers_signals.clear();
}

void gearoenix::vulkan::queue::Graph::SubmitData::clear() noexcept
{
    for (auto& frame : frame_data)
        frame.clear();
}

void gearoenix::vulkan::queue::Graph::SubmitData::add_provider(
    const std::vector<std::shared_ptr<sync::Semaphore>>& semaphores) noexcept
{
    if (frame_data.empty())
        frame_data.resize(semaphores.size());
    else
        GX_ASSERT_D(semaphores.size() == frame_data.size());
    for (std::size_t i = 0; i < frame_data.size(); ++i) {
        auto& frame = frame_data[i];
        auto& pw = frame.providers_waits;
        const auto& sem = semaphores[i];
        auto s = sem->get_vulkan_data();
        GX_ASSERT_D(std::find(pw.begin(), pw.end(), s) == pw.end());
        pw.push_back(s);
        frame.pipeline_stages.push_back(sem->get_pipeline_stage());
    }
}

void gearoenix::vulkan::queue::Graph::SubmitData::add_consumer(
    const std::vector<std::shared_ptr<sync::Semaphore>>& semaphores) noexcept
{
    if (frame_data.empty())
        frame_data.resize(semaphores.size());
    else
        GX_ASSERT_D(semaphores.size() == frame_data.size());
    for (std::size_t i = 0; i < frame_data.size(); ++i) {
        auto& cs = frame_data[i].consumers_signals;
        auto s = semaphores[i]->get_vulkan_data();
        GX_ASSERT_D(std::find(cs.begin(), cs.end(), s) == cs.end());
        cs.push_back(s);
    }
}

void gearoenix::vulkan::queue::Graph::update_traversing_level() noexcept
{
    auto& start_node = nodes.find(NodeLabel::Start)->second;
    start_node.clear_traversing_level(nodes);
    const auto max_tl = start_node.update_traversing_level(0, nodes);
    submit_data.resize(static_cast<std::size_t>(max_tl));
}

void gearoenix::vulkan::queue::Graph::clear_submit_data() noexcept
{
    for (auto& d : submit_data) {
        d.clear();
    }
}

void gearoenix::vulkan::queue::Graph::update_submit_data(Node& node) noexcept
{
    if (node.get_traversed())
        return;
    node.set_traversed(true);
    while (static_cast<std::size_t>(node.get_traversal_level()) + 1 > submit_data.size())
        submit_data.emplace_back();
    auto& sd = submit_data[node.get_traversal_level()];
    for (const NodeLabel provider_label : node.get_providers()) {
        const auto& provider_semaphores = nodes.find(provider_label)->second.get_consumers().find(node.get_node_label())->second;
        sd.add_provider(provider_semaphores);
    }
    for (const auto& consumer_label_semaphores : node.get_consumers()) {
        sd.add_consumer(consumer_label_semaphores.second);
    }
    for (std::size_t frame_index = 0; frame_index < sd.frame_data.size(); ++frame_index) {
        sd.frame_data[frame_index].cmd = node.get_frame_commands()[frame_index];
    }
}

void gearoenix::vulkan::queue::Graph::update() noexcept
{
    update_traversing_level();
    clear_submit_data();
    for (auto& node : nodes)
        update_submit_data(node.second);
    auto& front = submit_data.front();
    auto& back = submit_data.back();
    for (std::size_t frame_index = 0; frame_index < front.frame_data.size(); ++frame_index) {
        auto& front_frame = front.frame_data[frame_index];
        auto& back_frame = back.frame_data[frame_index];
        front_frame.providers_waits.push_back(start_semaphore[frame_index]->get_vulkan_data());
        front_frame.pipeline_stages.push_back(start_semaphore[frame_index]->get_pipeline_stage());
        back_frame.consumers_signals.push_back(end_semaphore[frame_index]->get_vulkan_data());
    }
}

void gearoenix::vulkan::queue::Graph::submit() noexcept
{
    const auto frame_index = q.get_e().get_frame_number();
    const auto submit_data_end = submit_data.size() - 1;
    for (std::size_t submit_data_index = 0; submit_data_index < submit_data_end; ++submit_data_index) {
        const auto& sd = submit_data[submit_data_index];
        const auto& frame = sd.frame_data[frame_index];
        q.submit(
            frame.providers_waits.size(), &(*frame.providers_waits.begin()),
            frame.pipeline_stages.data(),
            frame.cmd->get_has_record() ? 1 : 0, frame.cmd->get_vulkan_data_ptr(),
            frame.consumers_signals.size(), &(*frame.consumers_signals.begin()));
    }
    {
        const auto& sd = submit_data[submit_data_end];
        const auto& frame = sd.frame_data[frame_index];
        q.submit(
            frame.providers_waits.size(), &(*frame.providers_waits.begin()),
            frame.pipeline_stages.data(),
            frame.cmd->get_has_record() ? 1 : 0, frame.cmd->get_vulkan_data_ptr(),
            frame.consumers_signals.size(), &(*frame.consumers_signals.begin()),
            fence[frame_index]->get_vulkan_data());
    }
}

gearoenix::vulkan::queue::Graph::Graph(engine::Engine& e) noexcept
    : start_semaphore(sync::Semaphore::create_frame_based(e))
    , end_semaphore(sync::Semaphore::create_frame_based(e))
    , fence(sync::Fence::create_frame_based(e, true))
    , q(e)
{
    nodes.emplace(NodeLabel::Start, Node(e, NodeLabel::Start, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT));
    nodes.emplace(NodeLabel::End, Node(e, NodeLabel::End, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT));
    Node::connect(nodes.find(NodeLabel::Start)->second, nodes.find(NodeLabel::End)->second, e);
}

gearoenix::vulkan::queue::Graph::~Graph() noexcept = default;

std::vector<std::shared_ptr<gearoenix::vulkan::command::Buffer>> gearoenix::vulkan::queue::Graph::place_node_between(
    const NodeLabel provider_node_label,
    const NodeLabel new_node_label,
    const NodeLabel consumer_node_label,
    const VkPipelineStageFlags wait_stage) noexcept
{
    GX_CHECK_EQUAL_D(nodes.end(), nodes.find(new_node_label));
    auto& new_node = nodes.emplace(new_node_label, Node(q.get_e(), new_node_label, wait_stage)).first->second;

    auto provider_search = nodes.find(provider_node_label);
    GX_CHECK_NOT_EQUAL_D(nodes.end(), provider_search);

    auto consumer_search = nodes.find(consumer_node_label);
    GX_CHECK_NOT_EQUAL_D(nodes.end(), consumer_search);

    auto& provider_node = provider_search->second;
    auto& consumer_node = consumer_search->second;
    auto& provider_consumers = provider_node.get_consumers();
    auto& consumer_providers = consumer_node.get_providers();

    auto provider_old_consumer_search = provider_consumers.find(consumer_node_label);
    GX_CHECK_NOT_EQUAL_D(provider_consumers.end(), provider_old_consumer_search);

    auto consumer_old_provider_search = consumer_providers.find(provider_node_label);
    GX_CHECK_NOT_EQUAL_D(consumer_providers.end(), consumer_old_provider_search);

    provider_consumers.erase(provider_old_consumer_search);
    consumer_providers.erase(consumer_old_provider_search);

    Node::connect(provider_node, new_node, q.get_e());
    Node::connect(new_node, consumer_node, q.get_e());

    update();

    return new_node.get_frame_commands();
}

void gearoenix::vulkan::queue::Graph::start_frame() noexcept
{
    auto& f = *fence[q.get_e().get_frame_number()];
    f.wait();
    f.reset();
}

bool gearoenix::vulkan::queue::Graph::present(Swapchain& swapchain, const std::uint32_t image_index) noexcept
{
    submit();

    VkPresentInfoKHR info;
    GX_SET_ZERO(info);
    info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = end_semaphore[q.get_e().get_frame_number()]->get_vulkan_data_ptr();
    info.swapchainCount = 1;
    info.pSwapchains = swapchain.get_vulkan_data_ptr();
    info.pImageIndices = &image_index;
    const auto present_result = vkQueuePresentKHR(q.get_vulkan_data(), &info);
    if (VK_ERROR_OUT_OF_DATE_KHR == present_result) {
        return false;
    } else if (VK_SUCCESS != present_result) {
        GX_LOG_F("Presentation failed with result: " << result_to_string(present_result));
    }
    return true;
}

const gearoenix::vulkan::sync::Semaphore& gearoenix::vulkan::queue::Graph::get_present_semaphore() noexcept
{
    return *start_semaphore[q.get_e().get_frame_number()];
}

#endif
