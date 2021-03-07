#ifndef GEAROENIX_VULKAN_QUEUE_HPP
#define GEAROENIX_VULKAN_QUEUE_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../platform/macro/gx-plt-mcr-lock.hpp"
#include "../gx-vk-loader.hpp"
#include <map>
#include <memory>
#include <vector>

namespace gearoenix::vulkan {
struct Swapchain;
}

namespace gearoenix::vulkan::command {
struct Buffer;
}

namespace gearoenix::vulkan::engine {
struct Engine;
}

namespace gearoenix::vulkan::sync {
struct Fence;
struct Semaphore;
}

namespace gearoenix::vulkan::queue {
struct Queue final {
    constexpr static const auto* const START_FRAME = "start frame";
    constexpr static const auto* const END_FRAME = "end frame";

    GX_GET_CRRF_PRV(engine::Engine, e)
    GX_GET_VAL_PRV(VkQueue, vulkan_data, nullptr)

private:
    GX_CREATE_GUARD(this)

    struct Node final {
        const std::string name;
        const VkPipelineStageFlags wait_stage;
        const std::vector<std::shared_ptr<command::Buffer>> cmds;
        int traversal_level = -1;
        bool traversed = false;
        // semaphores belong to providers and only for performance they have been added in here
        std::map<std::string, std::pair<Node*, const std::vector<std::shared_ptr<sync::Semaphore>>>> providers;
        std::map<std::string, std::pair<Node*, const std::vector<std::shared_ptr<sync::Semaphore>>>> consumers;

        explicit Node(engine::Engine&, std::string name, VkPipelineStageFlags) noexcept;
        ~Node() noexcept;
        void clear_traversing_level() noexcept;
        void update_traversing_level(int) noexcept;

        static void connect(Node* provider, Node* consumer, engine::Engine& e) noexcept;
    };

    struct Graph final {
        const std::vector<std::shared_ptr<sync::Semaphore>> start_semaphore;
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

    Graph graph;

public:
    Queue(Queue&&) = delete;
    Queue(const Queue&) = delete;
    Queue& operator=(Queue&&) = delete;
    Queue& operator=(const Queue&) = delete;
    explicit Queue(engine::Engine& e) noexcept;
    ~Queue() noexcept;
    void submit(command::Buffer&, sync::Fence& fence) noexcept;
    void submit(
        sync::Semaphore& wait,
        command::Buffer& cmd,
        sync::Semaphore& signal,
        sync::Fence& fence,
        VkPipelineStageFlags wait_stage) noexcept;
    [[nodiscard]] bool present(
        sync::Semaphore& wait,
        Swapchain& swapchain,
        std::uint32_t image_index) noexcept;
    void submit() noexcept;
    void submit(
        std::size_t wait_semaphores_count,
        const VkSemaphore* wait_semaphores,
        std::size_t commands_count,
        const VkCommandBuffer* commands,
        std::size_t signal_semaphores_count,
        const VkSemaphore* signal_semaphores,
        VkPipelineStageFlags wait_stage) noexcept;
};
}

#endif
#endif