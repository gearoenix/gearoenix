#ifndef GEAROENIX_VULKAN_DEVICE_LOGICAL_HPP
#define GEAROENIX_VULKAN_DEVICE_LOGICAL_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"
#include <memory>

namespace gearoenix::vulkan::device {
struct Physical;
struct Logical final {
private:
    GX_GET_CRRF_PRV(Physical, physical_device)
    GX_GET_VAL_PRV(VkDevice, vulkan_data, nullptr)
    GX_GET_VAL_PRV(VkQueue, graphic_queue, nullptr)

public:
    explicit Logical(const Physical& p) noexcept;
    ~Logical() noexcept;
    void wait_to_finish() noexcept;
};
}
#endif
#endif
