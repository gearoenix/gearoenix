#ifndef GEAROENIX_VULKAN_SYNC_SEMAPHORE_HPP
#define GEAROENIX_VULKAN_SYNC_SEMAPHORE_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
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

public:
    Semaphore(Semaphore&&) noexcept;
    Semaphore(const Semaphore&) = delete;
    Semaphore& operator=(Semaphore&&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;
    explicit Semaphore(const device::Logical& logical_device) noexcept;
    ~Semaphore() noexcept;
    [[nodiscard]] const VkSemaphore* get_vulkan_data_ptr() const noexcept;
    [[nodiscard]] static std::vector<std::shared_ptr<Semaphore>> create_frame_based(const engine::Engine& e) noexcept;
};
}
#endif
#endif