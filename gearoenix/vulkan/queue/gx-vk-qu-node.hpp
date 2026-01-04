#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"
#include "gx-vk-qu-node-label.hpp"
#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>
#include <memory>
#include <string>
#include <vector>

namespace gearoenix::vulkan::command {
struct Buffer;
}

namespace gearoenix::vulkan::engine {
struct Engine;
}

namespace gearoenix::vulkan::sync {
struct Semaphore;
}

namespace gearoenix::vulkan::queue {
struct Node final {
    typedef boost::container::flat_map<NodeLabel, std::vector<std::shared_ptr<sync::Semaphore>> /*frame-semaphores*/> OutputMap;

    GX_GET_CREF_PRV(NodeLabel, node_label);
    GX_GET_VAL_PRV(VkPipelineStageFlags, stage, VK_PIPELINE_STAGE_NONE_KHR);
    GX_GET_CREF_PRV(std::vector<std::shared_ptr<command::Buffer>>, frame_commands);
    GX_GET_VAL_PRV(int, traversal_level, -1);
    GX_GETSET_VAL_PRV(bool, traversed, false);
    GX_GET_REF_PRV(boost::container::flat_set<NodeLabel>, providers);
    GX_GET_REF_PRV(OutputMap, consumers);

public:
    Node(engine::Engine&, NodeLabel node_label, VkPipelineStageFlags stage);
    ~Node();
    Node(Node&&);
    Node(const Node&);
    Node& operator=(Node&&);
    Node& operator=(const Node&);
    void clear_traversing_level(boost::container::flat_map<NodeLabel, Node>& nodes);
    [[nodiscard]] int update_traversing_level(int traversal_level, boost::container::flat_map<NodeLabel, Node>& nodes);
    static void connect(Node& provider, Node& consumer, engine::Engine& e);
};
}
#endif