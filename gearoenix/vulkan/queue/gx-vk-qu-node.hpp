#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
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

private:
    NodeLabel node_label;
    vk::PipelineStageFlags stage = { };
    std::vector<std::shared_ptr<command::Buffer>> frame_commands;
    int traversal_level = -1;
    bool traversed = false;
    boost::container::flat_set<NodeLabel> providers;
    output_map_t consumers;

public:
    [[nodiscard]] NodeLabel get_node_label() const { return node_label; }
    [[nodiscard]] vk::PipelineStageFlags get_stage() const { return stage; }
    [[nodiscard]] const std::vector<std::shared_ptr<command::Buffer>>& get_frame_commands() const { return frame_commands; }
    [[nodiscard]] int get_traversal_level() const { return traversal_level; }
    [[nodiscard]] bool get_traversed() const { return traversed; }
    void set_traversed(const bool v) { traversed = v; }
    [[nodiscard]] boost::container::flat_set<NodeLabel>& get_providers() { return providers; }
    [[nodiscard]] output_map_t& get_consumers() { return consumers; }

    Node(NodeLabel node_label, vk::PipelineStageFlags stage);
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
