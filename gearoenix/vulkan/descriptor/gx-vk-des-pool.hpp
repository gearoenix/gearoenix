#ifndef GEAROENIX_RENDER_DESCRIPTOR_POOL_HPP
#define GEAROENIX_RENDER_DESCRIPTOR_POOL_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"
#include <memory>

namespace gearoenix::vulkan::device {
struct Logical;
}

namespace gearoenix::vulkan::descriptor {
struct Manager;
struct PoolManager;
struct Set;
struct Pool final {
    friend struct PoolManager;
    friend struct Manager;

    GX_GET_CRRF_PRV(device::Logical, logical_device)
    GX_GET_VAL_PRV(VkDescriptorPool, vulkan_data, nullptr)

private:
    Pool(
        const device::Logical& logical_device,
        const VkDescriptorPoolSize* pool_sizes,
        std::uint32_t count) noexcept;

    [[nodiscard]] static Pool* create_imgui(const device::Logical& logical_device) noexcept;

public:
    Pool(Pool&&) = delete;
    Pool(const Pool&) = delete;
    Pool& operator=(Pool&&) = delete;
    Pool& operator=(const Pool&) = delete;
    ~Pool() noexcept;
};
}
#endif
#endif