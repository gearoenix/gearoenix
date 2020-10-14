#ifndef GEAROENIX_VULKAN_DEVICE_LOGICAL_HPP
#define GEAROENIX_VULKAN_DEVICE_LOGICAL_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../../core/gx-cr-static.hpp"
#include "../gx-vk-loader.hpp"
#include <memory>

namespace gearoenix::vulkan::device {
class Physical;
class Logical final {
private:
    GX_GET_REFC_PRV(std::shared_ptr<Physical>, physical_device)
    GX_GET_VAL_PRV(VkDevice, vulkan_data, nullptr)
    GX_GET_VAL_PRV(VkQueue, graphic_queue, nullptr)

public:
    explicit Logical(std::shared_ptr<Physical> p) noexcept;
    ~Logical() noexcept;
    void wait_to_finish() noexcept;
};
}
#endif
#endif
