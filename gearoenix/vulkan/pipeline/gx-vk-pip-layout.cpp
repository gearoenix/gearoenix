#include "gx-vk-pip-layout.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../descriptor/gx-vk-des-set-layout.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../gx-vk-check.hpp"

gearoenix::vulkan::pipeline::Layout::Layout(std::shared_ptr<descriptor::SetLayout> _des_set_lay) noexcept
    : des_set_layout(std::move(_des_set_lay))
{
    VkPipelineLayoutCreateInfo info;
    GX_SET_ZERO(info)
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    info.setLayoutCount = 1;
    info.pSetLayouts = des_set_layout->get_vulkan_data_ptr();
    GX_VK_CHK(vkCreatePipelineLayout(des_set_layout->get_logical_device().get_vulkan_data(), &info, nullptr, &vulkan_data))
}

gearoenix::vulkan::pipeline::Layout::~Layout() noexcept
{
    vkDestroyPipelineLayout(des_set_layout->get_logical_device().get_vulkan_data(), vulkan_data, nullptr);
}
#endif
