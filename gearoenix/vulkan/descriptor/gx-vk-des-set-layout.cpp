#include "gx-vk-des-set-layout.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../gx-vk-check.hpp"

gearoenix::vulkan::descriptor::SetLayout::SetLayout(
    const device::Logical& logical_device, const std::vector<VkDescriptorSetLayoutBinding>& data) noexcept
    : logical_device(logical_device)
{
    VkDescriptorSetLayoutCreateInfo info;
    GX_SET_ZERO(info)
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    info.bindingCount = static_cast<std::uint32_t>(data.size());
    info.pBindings = data.data();
    GX_VK_CHK(vkCreateDescriptorSetLayout(logical_device.get_vulkan_data(), &info, nullptr, &vulkan_data))
}

gearoenix::vulkan::descriptor::SetLayout::~SetLayout() noexcept
{
    vkDestroyDescriptorSetLayout(logical_device.get_vulkan_data(), vulkan_data, nullptr);
}

VkDescriptorSetLayout* gearoenix::vulkan::descriptor::SetLayout::get_vulkan_data_ptr() noexcept
{
    return &vulkan_data;
}

#endif
