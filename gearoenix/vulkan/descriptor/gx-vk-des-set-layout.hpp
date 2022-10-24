#ifndef GEAROENIX_VULKAN_DESCRIPTOR_SET_LAYOUT_HPP
#define GEAROENIX_VULKAN_DESCRIPTOR_SET_LAYOUT_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"
#include <vector>

namespace gearoenix::vulkan::device {
struct Logical;
}

namespace gearoenix::vulkan::descriptor {
struct SetLayout final {
    GX_GET_VAL_PRV(VkDescriptorSetLayout, vulkan_data, nullptr);
    GX_GET_CRRF_PRV(device::Logical, logical_device);

public:
    SetLayout(SetLayout&&) = delete;
    SetLayout(const SetLayout&) = delete;
    SetLayout& operator=(SetLayout&&) = delete;
    SetLayout& operator=(const SetLayout&) = delete;
    SetLayout(const device::Logical& logical_device, const std::vector<VkDescriptorSetLayoutBinding>& data) noexcept;
    ~SetLayout() noexcept;
    [[nodiscard]] VkDescriptorSetLayout* get_vulkan_data_ptr() noexcept;
};
}
#endif
#endif