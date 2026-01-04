#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"

#include <memory>
#include <vector>

namespace gearoenix::vulkan::device {
struct Logical;
}

namespace gearoenix::vulkan::engine {
struct Engine;
}

namespace gearoenix::vulkan::sync {
struct Semaphore final {
    GX_GET_CRRF_PRV(device::Logical, logical_device);
    GX_GET_VAL_PRV(VkSemaphore, vulkan_data, nullptr);
    GX_GETSET_VAL_PRV(VkPipelineStageFlags, pipeline_stage, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);

public:
    Semaphore(Semaphore&&) noexcept;
    Semaphore(const Semaphore&) = delete;
    Semaphore& operator=(Semaphore&&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;
    explicit Semaphore(const device::Logical& logical_device);
    ~Semaphore();
    [[nodiscard]] const VkSemaphore* get_vulkan_data_ptr() const;
    [[nodiscard]] static std::vector<std::shared_ptr<Semaphore>> create_frame_based(const engine::Engine& e);
};
}
#endif