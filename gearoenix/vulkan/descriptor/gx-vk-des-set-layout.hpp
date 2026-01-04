#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
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
    SetLayout(
        const device::Logical& logical_device,
        const std::vector<VkDescriptorSetLayoutBinding>& data,
        VkDescriptorSetLayoutCreateFlags flags = 0,
        const std::vector<VkDescriptorBindingFlags>& binding_flags = {});
    ~SetLayout();
    [[nodiscard]] VkDescriptorSetLayout* get_vulkan_data_ptr();
};
}
#endif