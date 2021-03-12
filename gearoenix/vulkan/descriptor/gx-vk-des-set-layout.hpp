#ifndef GEAROENIX_VULKAN_DESCRIPTOR_SET_LAYOUT_HPP
#define GEAROENIX_VULKAN_DESCRIPTOR_SET_LAYOUT_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../gx-vk-loader.hpp"

namespace gearoenix::vulkan::device {
struct Logical;
}

namespace gearoenix::vulkan::descriptor {
struct SetLayout {
private:
    const device::Logical& logical_device;
    VkDescriptorSetLayout vulkan_data = nullptr;

public:
    SetLayout(const device::Logical& logical_device);
    ~SetLayout();
    const device::Logical* get_logical_device() const;
    const VkDescriptorSetLayout& get_vulkan_data() const;
};
}
#endif
#endif