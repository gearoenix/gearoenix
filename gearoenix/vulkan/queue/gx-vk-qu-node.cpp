#include "gx-vk-qu-node.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../sync/gx-vk-sync-semaphore.hpp"

gearoenix::vulkan::queue::Node::Node(engine::Engine& e, const NodeLabel node_label, VkPipelineStageFlags stage) noexcept
    : node_label(node_label)
    , stage(stage)
    , frame_commands(e.get_command_manager().create_frame_based())
{
}

gearoenix::vulkan::queue::Node::~Node() noexcept = default;

gearoenix::vulkan::queue::Node::Node(Node&& o) noexcept = default;

gearoenix::vulkan::queue::Node::Node(const Node&) noexcept = default;

gearoenix::vulkan::queue::Node& gearoenix::vulkan::queue::Node::operator=(Node&& o) noexcept = default;

gearoenix::vulkan::queue::Node& gearoenix::vulkan::queue::Node::operator=(const Node& o) noexcept = default;

void gearoenix::vulkan::queue::Node::clear_traversing_level(boost::container::flat_map<NodeLabel, Node>& nodes) noexcept
{
    traversal_level = -1;
    traversed = false;
    for (auto& consumer : consumers) {
        auto node_search = nodes.find(consumer.first);
        GX_ASSERT(nodes.end() != node_search);
        node_search->second.clear_traversing_level(nodes);
    }
}

int gearoenix::vulkan::queue::Node::update_traversing_level(const int tl, boost::container::flat_map<NodeLabel, Node>& nodes) noexcept
{
    int max_tl = tl;
    if (tl > traversal_level) {
        traversal_level = tl;
        for (auto& consumer : consumers) {
            auto node_search = nodes.find(consumer.first);
            GX_ASSERT(nodes.end() != node_search);
            const auto child_max_tl = node_search->second.update_traversing_level(tl + 1, nodes);
            if (child_max_tl > max_tl)
                max_tl = child_max_tl;
        }
    }
    return max_tl;
}

void gearoenix::vulkan::queue::Node::connect(
    Node& provider,
    Node& consumer,
    engine::Engine& e) noexcept
{
    GX_CHECK_EQUAL_D(consumer.providers.end(), consumer.providers.find(provider.node_label));
    GX_CHECK_EQUAL_D(provider.consumers.end(), provider.consumers.find(consumer.node_label));

    consumer.providers.insert(provider.node_label);
    provider.consumers.emplace(consumer.node_label, sync::Semaphore::create_frame_based(e));
}

#endif