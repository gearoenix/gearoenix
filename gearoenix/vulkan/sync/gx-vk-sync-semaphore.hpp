#ifndef GEAROENIX_VULKAN_SYNC_SEMAPHORE_HPP
#define GEAROENIX_VULKAN_SYNC_SEMAPHORE_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../../core/gx-cr-static.hpp"
#include "../gx-vk-loader.hpp"

namespace gearoenix::vulkan::device {
class Logical;
}

namespace gearoenix::vulkan::sync {
class Semaphore final {
    GX_GET_REFC_PRV(std::shared_ptr<device::Logical>, logical_device)
    GX_GET_VAL_PRV(VkSemaphore, vulkan_data, nullptr)

public:
    explicit Semaphore(std::shared_ptr<device::Logical> logical_device) noexcept;
    ~Semaphore() noexcept;
};
}
#endif
#endif