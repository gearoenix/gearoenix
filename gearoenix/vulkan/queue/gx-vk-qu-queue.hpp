#ifndef GEAROENIX_VULKAN_QUEUE_HPP
#define GEAROENIX_VULKAN_QUEUE_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
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
    GX_GET_RRF_PRV(engine::Engine, e);
    GX_GET_VAL_PRV(VkQueue, vulkan_data, nullptr);

public:
    Queue(Queue&&) = delete;
    Queue(const Queue&) = delete;
    Queue& operator=(Queue&&) = delete;
    Queue& operator=(const Queue&) = delete;
    /// It must be created in the same thread that it is going to be used.
    explicit Queue(engine::Engine& e) noexcept;
    ~Queue() noexcept;
    void submit(command::Buffer&, sync::Fence& fence) noexcept;
    void submit(
        std::size_t wait_semaphores_count,
        const VkSemaphore* wait_semaphores,
        const VkPipelineStageFlags* wait_stages,
        std::size_t commands_count,
        const VkCommandBuffer* commands,
        std::size_t signal_semaphores_count,
        const VkSemaphore* signal_semaphores,
        VkFence fence = nullptr) noexcept;
};
}

#endif
#endif