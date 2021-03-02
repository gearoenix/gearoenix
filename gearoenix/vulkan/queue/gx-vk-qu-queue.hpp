#ifndef GEAROENIX_VULKAN_QUEUE_HPP
#define GEAROENIX_VULKAN_QUEUE_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../platform/macro/gx-plt-mcr-lock.hpp"
#include "../gx-vk-loader.hpp"

namespace gearoenix::vulkan::device {
struct Logical;
}

namespace gearoenix::vulkan::command {
struct Buffer;
}

namespace gearoenix::vulkan::queue {
struct Queue final {
    GX_GET_CRRF_PRV(device::Logical, logical_device)
    GX_GET_VAL_PRV(VkQueue, vulkan_data, nullptr)

private:
    GX_CREATE_GUARD(submit)

public:
    Queue(const device::Logical& logical_device, VkQueue vulkan_data) noexcept;
    Queue(const device::Logical& logical_device, std::uint32_t node_index) noexcept;
    ~Queue() noexcept;
    Queue(Queue&&) = delete;
    Queue(const Queue&) = delete;
    Queue& operator=(Queue&&) = delete;
    Queue& operator=(const Queue&) = delete;
};
}

#endif
#endif