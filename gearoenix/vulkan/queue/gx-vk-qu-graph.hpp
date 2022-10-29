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

            void clear() noexcept;
        };
        std::vector<FrameData> frame_data;

        void clear() noexcept;
        void add_provider(const std::vector<std::shared_ptr<sync::Semaphore>>& semaphores) noexcept;
        void add_consumer(const std::vector<std::shared_ptr<sync::Semaphore>>& semaphores) noexcept;
    };

    const std::vector<std::shared_ptr<sync::Semaphore>> start_semaphore;
    const std::vector<std::shared_ptr<sync::Semaphore>> end_semaphore;
    const std::vector<std::shared_ptr<sync::Fence>> fence;

    GX_GET_CREF_PRV(Queue, q);
    GX_GET_CREF_PRV(NodeMap, nodes);
    GX_GET_CREF_PRV(std::vector<SubmitData>, submit_data);

private:
    void update_traversing_level() noexcept;
    void clear_submit_data() noexcept;
    void update_submit_data(Node& node) noexcept;
    void update() noexcept;
    void submit() noexcept;

public:
    explicit Graph(engine::Engine&) noexcept;
    ~Graph() noexcept;
    [[nodiscard]] std::vector<std::shared_ptr<command::Buffer>> place_node_between(
        NodeLabel provider_node_label,
        NodeLabel new_node_label,
        NodeLabel consumer_node_label,
        VkPipelineStageFlags wait_stage) noexcept;
    [[nodiscard]] std::vector<std::shared_ptr<command::Buffer>> create_node_path(
        NodeLabel provider_node_label,
        NodeLabel new_node_label,
        NodeLabel consumer_node_label,
        VkPipelineStageFlags wait_stage) noexcept;
    void start_frame() noexcept;
    [[nodiscard]] bool present(Swapchain& swapchain, std::uint32_t image_index) noexcept;
    [[nodiscard]] const sync::Semaphore& get_present_semaphore() noexcept;
};
}

#endif
#endif