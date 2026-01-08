#include "gx-vk-pip-layout.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../descriptor/gx-vk-des-set-layout.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../gx-vk-check.hpp"

#include <ranges>

gearoenix::vulkan::pipeline::Layout::Layout(std::vector<std::shared_ptr<descriptor::SetLayout>>&& layouts)
    : des_set_layouts(std::move(layouts))
{
    const auto sets =
            des_set_layouts
            | std::views::transform([](const auto& l) { return l->get_vulkan_data(); })
            | std::ranges::to<std::vector<VkDescriptorSetLayout>>();

    VkPipelineLayoutCreateInfo info;
    GX_SET_ZERO(info);
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    info.setLayoutCount = sets.size();
    info.pSetLayouts = sets.data();
    GX_VK_CHK(vkCreatePipelineLayout(device::Logical::get().get_vulkan_data(), &info, nullptr, &vulkan_data));
}

gearoenix::vulkan::pipeline::Layout::~Layout()
{
    vkDestroyPipelineLayout(device::Logical::get().get_vulkan_data(), vulkan_data, nullptr);
}
#endif
