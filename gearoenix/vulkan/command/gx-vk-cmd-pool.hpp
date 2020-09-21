#ifndef GEAROENIX_VULKAN_COMMAND_POOL_HPP
#define GEAROENIX_VULKAN_COMMAND_POOL_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../../core/gx-cr-static.hpp"
#include "../gx-vk-loader.hpp"

namespace gearoenix::vulkan::device {
class Logical;
}

namespace gearoenix::vulkan::command {
class Pool final {
    GX_GET_REFC_PRV(std::shared_ptr<device::Logical>, logical_device)
    GX_GET_VAL_PRV(VkCommandPool, vulkan_data, nullptr)

public:
    explicit Pool(std::shared_ptr<device::Logical> logical_device) noexcept;
    ~Pool() noexcept;
};
}
#endif
#endif
