#ifndef GEAROENIX_VULKAN_POOL_BUFFER_HPP
#define GEAROENIX_VULKAN_POOL_BUFFER_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../gx-vk-loader.hpp"
#include <memory>
#include <vector>

namespace gearoenix::vulkan::device {
class Logical;
}

namespace gearoenix::vulkan::command {
class Pool final {
private:
    std::shared_ptr<device::Logical> logical_device;
    VkCommandPool vulkan_data;

public:
    explicit Pool(std::shared_ptr<device::Logical> logical_device) noexcept;
    ~Pool() noexcept;
    [[nodiscard]] VkCommandPool get_vulkan_data() const;
    [[nodiscard]] const std::shared_ptr<device::Logical> get_logical_device() const;
};
}
#endif
#endif
