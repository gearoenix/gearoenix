#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../core/gx-cr-singleton.hpp"
#include "../gx-vk-loader.hpp"


namespace gearoenix::vulkan::command {
struct Buffer;
}

namespace gearoenix::vulkan::sync {
struct Fence;
struct Semaphore;
}

namespace gearoenix::vulkan::queue {
/// We only have one queue because some devices only have support for one queue.
/// Even those devices, which support several queues, don't have a good parallelism.
struct Queue final : core::Singleton<Queue> {
    GX_GET_CVAL_PRV(VkQueue, vulkan_data);

public:
    Queue(Queue&&) = delete;
    Queue(const Queue&) = delete;
    Queue& operator=(Queue&&) = delete;
    Queue& operator=(const Queue&) = delete;
    /// It must be created in the same thread that it is going to be used.
    Queue();
    ~Queue() override;
    void submit(const command::Buffer&, const sync::Fence& fence);
    void submit(
        std::uint64_t wait_semaphores_count,
        const VkSemaphore* wait_semaphores,
        const VkPipelineStageFlags* wait_stages,
        std::uint64_t commands_count,
        const VkCommandBuffer* commands,
        std::uint64_t signal_semaphores_count,
        const VkSemaphore* signal_semaphores,
        VkFence fence = nullptr);
    void update();
};
}
#endif