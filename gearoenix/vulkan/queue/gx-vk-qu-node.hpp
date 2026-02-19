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
    using per_frame_semaphores_t = std::vector<std::shared_ptr<sync::Semaphore>>;
    using output_map_t = boost::container::flat_map<NodeLabel, per_frame_semaphores_t>;

    GX_GET_CREF_PRV(NodeLabel, node_label);
    GX_GET_VAL_PRV(VkPipelineStageFlags, stage, VK_PIPELINE_STAGE_NONE_KHR);
    GX_GET_CREF_PRV(std::vector<std::shared_ptr<command::Buffer>>, frame_commands);
    GX_GET_VAL_PRV(int, traversal_level, -1);
    GX_GETSET_VAL_PRV(bool, traversed, false);
    GX_GET_REF_PRV(boost::container::flat_set<NodeLabel>, providers);
    GX_GET_REF_PRV(output_map_t, consumers);

public:
    Node(NodeLabel node_label, VkPipelineStageFlags stage);
    ~Node();
    Node(Node&&) noexcept;
    Node(const Node&);
    Node& operator=(Node&&) noexcept;
    Node& operator=(const Node&);
    void clear_traversing_level(boost::container::flat_map<NodeLabel, Node>& nodes);
    [[nodiscard]] int update_traversing_level(int tl, boost::container::flat_map<NodeLabel, Node>& nodes);
    static void connect(Node& provider, Node& consumer);
};
}
#endif