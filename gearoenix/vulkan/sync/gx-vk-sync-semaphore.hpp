#ifndef GEAROENIX_VULKAN_SYNC_SEMAPHORE_HPP
#define GEAROENIX_VULKAN_SYNC_SEMAPHORE_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../render/sync/gx-rnd-sy-semaphore.hpp"
#include "../gx-vk-loader.hpp"

namespace gearoenix::vulkan::device {
struct Logical;
}

namespace gearoenix::vulkan::sync {
struct Semaphore final : public render::sync::Semaphore {
    GX_GET_REFC_PRV(std::shared_ptr<device::Logical>, logical_device)
    GX_GET_VAL_PRV(VkSemaphore, vulkan_data, nullptr)

public:
    explicit Semaphore(std::shared_ptr<device::Logical> logical_device) noexcept;
    ~Semaphore() noexcept final;
};
}
#endif
#endif