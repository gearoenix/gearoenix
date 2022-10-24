#include "gx-vk-qu-graph.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../command/gx-vk-cmd-buffer.hpp"
#include "../sync/gx-vk-sync-fence.hpp"
#include "../sync/gx-vk-sync-semaphore.hpp"

void gearoenix::vulkan::queue::Graph::SubmitData::FrameData::clear() noexcept
{
    waits.clear();
    cmd = nullptr;
    signals.clear();
}

void gearoenix::vulkan::queue::Graph::SubmitData::clear() noexcept
{
    pipeline_stage = 0;
    for (auto& frame : frame_data)
        frame.clear();
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
        sd.add_provider(provider);
    }
    for (const auto& consumer_label_semaphores : node.get_consumers()) {
        sd.add_consumer(consumer_label_semaphores.second);
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
        front_frame.waits.insert(start_semaphore[frame_index]->get_vulkan_data());
        back_frame.waits.insert(end_semaphore[frame_index]->get_vulkan_data());
    }
    front.pipeline_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
}

gearoenix::vulkan::queue::Graph::Graph(engine::Engine& e) noexcept
    : e(e)
    , start_semaphore(sync::Semaphore::create_frame_based(e))
    , end_semaphore(sync::Semaphore::create_frame_based(e))
    , fence(sync::Fence::create_frame_based(e, true))
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
    auto& new_node = nodes.emplace(new_node_label, Node(e, new_node_label, wait_stage)).first->second;

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

    Node::connect(provider_node, new_node, e);
    Node::connect(new_node, consumer_node, e);

    update();

    return new_node.get_frame_commands();
}

#endif
