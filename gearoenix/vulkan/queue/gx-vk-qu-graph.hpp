#ifndef GEAROENIX_VULKAN_QUEUE_GRAPH_HPP
#define GEAROENIX_VULKAN_QUEUE_GRAPH_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "gx-vk-qu-node-label.hpp"
#include "gx-vk-qu-node.hpp"
#include "gx-vk-qu-queue.hpp"
#include <atomic>
#include <boost/container/flat_set.hpp>
#include <string>

namespace gearoenix::vulkan::sync {
struct Fence;
}

namespace gearoenix::vulkan::queue {
struct Graph final {
    typedef boost::container::flat_map<NodeLabel, Node> NodeMap;

    struct SubmitData final {
        struct FrameData final {
            std::vector<VkSemaphore> providers_waits;
            std::vector<VkPipelineStageFlags> pipeline_stages;
            std::shared_ptr<command::Buffer> cmd;
            std::vector<VkSemaphore> consumers_signals;

            void clear();
        };
        std::vector<FrameData> frame_data;

        void clear();
        void add_provider(const std::vector<std::shared_ptr<sync::Semaphore>>& semaphores);
        void add_consumer(const std::vector<std::shared_ptr<sync::Semaphore>>& semaphores);
    };

    const std::vector<std::shared_ptr<sync::Semaphore>> start_semaphore;
    const std::vector<std::shared_ptr<sync::Semaphore>> end_semaphore;
    const std::vector<std::shared_ptr<sync::Fence>> fence;

    GX_GET_CREF_PRV(Queue, q);
    GX_GET_CREF_PRV(NodeMap, nodes);
    GX_GET_CREF_PRV(std::vector<SubmitData>, submit_data);

private:
    void update_traversing_level();
    void clear_submit_data();
    void update_submit_data(Node& node);
    void update();
    void submit();

public:
    explicit Graph(engine::Engine&);
    ~Graph();
    [[nodiscard]] std::vector<std::shared_ptr<command::Buffer>> place_node_between(
        NodeLabel provider_node_label,
        NodeLabel new_node_label,
        NodeLabel consumer_node_label,
        VkPipelineStageFlags wait_stage);
    [[nodiscard]] std::vector<std::shared_ptr<command::Buffer>> create_node_path(
        NodeLabel provider_node_label,
        NodeLabel new_node_label,
        NodeLabel consumer_node_label,
        VkPipelineStageFlags wait_stage);
    void start_frame();
    [[nodiscard]] bool present(Swapchain& swapchain, std::uint32_t image_index);
    [[nodiscard]] const sync::Semaphore& get_present_semaphore();
};
}

#endif
#endif