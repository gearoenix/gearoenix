#include "gx-vk-des-set-layout.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../gx-vk-check.hpp"

gearoenix::vulkan::descriptor::SetLayout::SetLayout(
    const std::vector<VkDescriptorSetLayoutBinding>& data,
    const VkDescriptorSetLayoutCreateFlags flags,
    const std::vector<VkDescriptorBindingFlags>& binding_flags)
{
    VkDescriptorSetLayoutBindingFlagsCreateInfo binding_flags_info;
    GX_SET_ZERO(binding_flags_info);

    VkDescriptorSetLayoutCreateInfo info;
    GX_SET_ZERO(info);
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    info.bindingCount = static_cast<std::uint32_t>(data.size());
    info.pBindings = data.data();
    info.flags = flags;

    if (!binding_flags.empty()) {
        GX_ASSERT_D(binding_flags.size() == data.size());
        binding_flags_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
        binding_flags_info.bindingCount = static_cast<std::uint32_t>(binding_flags.size());
        binding_flags_info.pBindingFlags = binding_flags.data();
        info.pNext = &binding_flags_info;
    }

    GX_VK_CHK(vkCreateDescriptorSetLayout(device::Logical::get().get_vulkan_data(), &info, nullptr, &vulkan_data));
}

gearoenix::vulkan::descriptor::SetLayout::~SetLayout()
{
    vkDestroyDescriptorSetLayout(device::Logical::get().get_vulkan_data(), vulkan_data, nullptr);
}

VkDescriptorSetLayout* gearoenix::vulkan::descriptor::SetLayout::get_vulkan_data_ptr()
{
    return &vulkan_data;
}

#endif
