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
struct Graph;
struct Queue final {
    constexpr static const auto* const START_FRAME = "start frame";
    constexpr static const auto* const END_FRAME = "end frame";

    GX_GET_RRF_PRV(engine::Engine, e)
    GX_GET_VAL_PRV(VkQueue, vulkan_data, nullptr)

private:
    GX_CREATE_GUARD(this)
    const std::unique_ptr<Graph> graph;

    void submit(
        std::size_t wait_semaphores_count,
        const VkSemaphore* wait_semaphores,
        std::size_t commands_count,
        const VkCommandBuffer* commands,
        std::size_t signal_semaphores_count,
        const VkSemaphore* signal_semaphores,
        VkPipelineStageFlags wait_stage,
        VkFence fence = nullptr) noexcept;
    void submit() noexcept;

public:
    Queue(Queue&&) = delete;
    Queue(const Queue&) = delete;
    Queue& operator=(Queue&&) = delete;
    Queue& operator=(const Queue&) = delete;

    explicit Queue(engine::Engine& e) noexcept;
    ~Queue() noexcept;
    void submit(command::Buffer&, sync::Fence& fence) noexcept;
    void start_frame() noexcept;
    [[nodiscard]] bool present(
        Swapchain& swapchain,
        std::uint32_t image_index) noexcept;
    [[nodiscard]] const sync::Semaphore& get_present_semaphore() noexcept;
    [[nodiscard]] std::vector<std::shared_ptr<command::Buffer>> place_node_between(
        const std::string& previous_node_name,
        const std::string& node_name,
        VkPipelineStageFlags wait_stage,
        const std::string& next_node_name) noexcept;
    [[nodiscard]] std::vector<std::shared_ptr<command::Buffer>> create_node_path(
        const std::string& previous_node_name,
        const std::string& node_name,
        VkPipelineStageFlags wait_stage,
        const std::string& next_node_name) noexcept;
};
}

#endif
#endif