#ifndef GEAROENIX_RENDER_DESCRIPTOR_POOL_HPP
#define GEAROENIX_RENDER_DESCRIPTOR_POOL_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"

namespace gearoenix::vulkan::device {
struct Logical;
}

namespace gearoenix::vulkan::descriptor {
struct Pool final {
    GX_GET_CRRF_PRV(device::Logical, logical_device)
    GX_GET_VAL_PRV(VkDescriptorPool, vulkan_data, nullptr)

    Pool(const device::Logical& logical_device, VkDescriptorPool vulkan_data) noexcept;

public:
    Pool(const Pool&) = delete;
    Pool(Pool&&) = delete;
    explicit Pool(const device::Logical& logical_device) noexcept;
    [[nodiscard]] static Pool create_imgui(const device::Logical& logical_device) noexcept;
    ~Pool() noexcept;
    Pool& operator=(const Pool&) = delete;
    Pool& operator=(Pool&&) = delete;
};
}
#endif
#endif