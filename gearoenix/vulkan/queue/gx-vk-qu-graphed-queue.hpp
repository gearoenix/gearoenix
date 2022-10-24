#ifndef GEAROENIX_VULKAN_GRAPHED_QUEUE_HPP
#define GEAROENIX_VULKAN_GRAPHED_QUEUE_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "gx-vk-qu-graph.hpp"
#include "gx-vk-qu-queue.hpp"

namespace gearoenix::vulkan::queue {
struct GraphedQueue final {
    GX_GET_CREF_PRV(Graph, graph);
    GX_GET_CREF_PRV(Queue, qu);

private:
    void submit() noexcept;

public:
    explicit GraphedQueue(engine::Engine& e) noexcept;
    ~GraphedQueue() noexcept;
    void start_frame() noexcept;
    [[nodiscard]] bool present(Swapchain& swapchain, std::uint32_t image_index) noexcept;
    [[nodiscard]] const sync::Semaphore& get_present_semaphore() noexcept;
};
}

#endif
#endif