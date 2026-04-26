#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
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
private:
    vk::Queue vulkan_data;
    std::mutex submission_lock;

public:
    [[nodiscard]] vk::Queue get_vulkan_data() const { return vulkan_data; }

    Queue(Queue&&) = delete;
    Queue(const Queue&) = delete;
    Queue& operator=(Queue&&) = delete;
    Queue& operator=(const Queue&) = delete;
    /// It must be created in the same thread that it is going to be used.
    Queue();
    ~Queue() override;
    void submit(const command::Buffer&, const sync::Fence& fence);
    void submit(
        vk::ArrayProxy<const vk::Semaphore> wait_semaphores,
        vk::ArrayProxy<const vk::PipelineStageFlags> wait_stages,
        vk::ArrayProxy<const vk::CommandBuffer> commands,
        vk::ArrayProxy<const vk::Semaphore> signal_semaphores,
        vk::Fence fence = nullptr);
    /// Raw access to the submission mutex. Any code that touches the queue through
    /// `get_vulkan_data()` (e.g. ImGui's backend calling `vkQueueSubmit` / `vkQueueWaitIdle`
    /// inside `RenderDrawData`) must take this lock locally via `std::lock_guard`/
    /// `std::scoped_lock` so its ops are serialised with the `submit` / `present` paths
    /// above. Exposing the mutex directly (rather than returning a guard from here) keeps
    /// the lock's lifetime fully in the caller's scope — no reliance on copy-elision of a
    /// non-movable `std::lock_guard` across a function return.
    [[nodiscard]] std::mutex& get_submission_lock() { return submission_lock; }
    void present(const vk::PresentInfoKHR& info);
};
}
#endif
