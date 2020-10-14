#include "gx-vk-pip-layout.hpp"
#ifdef USE_VULKAN
#include "../../core/gx-cr-static.hpp"
#include "../descriptor/gx-vk-des-pool.hpp"
#include "../descriptor/gx-vk-des-set-layout.hpp"
#include "../descriptor/gx-vk-des-set.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../gx-vk-check.hpp"
#include "../gx-vk-instance.hpp"

gearoenix::render::pipeline::Layout::Layout(descriptor::SetLayout* dessetlay)
    : dessetlay(dessetlay)
{
    const device::Logical* dev = dessetlay->get_logical_device();
    const device::Physical* p = dev->get_physical_device();
    const Linker* l = p->get_instance()->get_linker();
    VkPipelineLayoutCreateInfo pipeline_layout_create_info;
    setz(pipeline_layout_create_info);
    pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_create_info.setLayoutCount = 1;
    pipeline_layout_create_info.pSetLayouts = &dessetlay->get_vulkan_data();
    VKC(l->vkCreatePipelineLayout(dev->get_vulkan_data(),
        &pipeline_layout_create_info, nullptr,
        &vulkan_data));
}

gearoenix::render::pipeline::Layout::~Layout()
{
    const device::Logical* dev = dessetlay->get_logical_device();
    const Linker* l = dev->get_physical_device()->get_instance()->get_linker();
    l->vkDestroyPipelineLayout(dev->get_vulkan_data(), vulkan_data,
        nullptr);
}

const VkPipelineLayout& gearoenix::render::pipeline::Layout::get_vulkan_data() const
{
    return vulkan_data;
}

const gearoenix::render::descriptor::SetLayout* gearoenix::render::pipeline::Layout::get_descriptor_set_layout() const
{
    return dessetlay;
}
#endif
