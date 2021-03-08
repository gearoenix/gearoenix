#ifndef GEAROENIX_VULKAN_QUEUE_GRAPH_HPP
#define GEAROENIX_VULKAN_QUEUE_GRAPH_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "gx-vk-qu-node.hpp"

namespace gearoenix::vulkan::sync {
struct Fence;
}

namespace gearoenix::vulkan::queue {
struct Graph final {
    const std::vector<std::shared_ptr<sync::Semaphore>> start_semaphore;
    const std::vector<std::shared_ptr<sync::Semaphore>> end_semaphore;
    const std::vector<std::shared_ptr<sync::Fence>> fence;
    std::map<std::string, Node> nodes;
    Node* const start;
    Node* const end;

    /// traversal-level -> pipeline-stage -> (waits, cmds, signals)
    std::vector<std::map<VkPipelineStageFlags,
        std::tuple<std::vector<VkSemaphore>, std::vector<VkCommandBuffer>, std::vector<VkSemaphore>>>>
        submit_data;

    explicit Graph(engine::Engine&) noexcept;
    ~Graph() noexcept;

    void update_traversing_level() noexcept;
    void clear_submit_data() noexcept;
    void update_submit_data(Node&, std::size_t frame_number) noexcept;
    void update(std::size_t frame_number) noexcept;
};
}

#endif
#endif