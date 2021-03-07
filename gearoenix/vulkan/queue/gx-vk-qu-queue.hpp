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
        std::vector<std::shared_ptr<command::Buffer>> cmds;
        std::map<std::string, Node*> providers;
        std::map<std::string, std::pair<Node*, std::vector<std::shared_ptr<sync::Semaphore>>>> consumers;

        explicit Node(const engine::Engine&) noexcept;
        ~Node() noexcept;
    };

    struct Graph final {
        std::vector<std::shared_ptr<sync::Semaphore>> present;
        std::shared_ptr<Node> start;
        std::shared_ptr<Node> end;
        std::map<std::string, std::shared_ptr<Node>> nodes;

        explicit Graph(const engine::Engine&) noexcept;
        ~Graph() noexcept;
    };

    Graph graph;

public:
    Queue(Queue&&) = delete;
    Queue(const Queue&) = delete;
    Queue& operator=(Queue&&) = delete;
    Queue& operator=(const Queue&) = delete;
    explicit Queue(const engine::Engine& e) noexcept;
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
};
}

#endif
#endif