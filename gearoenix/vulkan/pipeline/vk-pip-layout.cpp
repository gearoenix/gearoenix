#include "vk-pip-layout.hpp"
#include "../../core/cr-static.hpp"
#include "../descriptor/vk-des-pool.hpp"
#include "../descriptor/vk-des-set.hpp"
#include "../device/vk-dev-logical.hpp"
#include "../device/vk-dev-physical.hpp"
#include "../vk-check.hpp"
#include "../vk-instance.hpp"

gearoenix::render::pipeline::Layout::Layout(const std::shared_ptr<descriptor::Set>& desset)
    : dev(desset->get_pool()->get_logical_device())
    , desset(desset)
{
    const device::Physical* p = dev->get_physical_device();
    const Linker* l = p->get_instance()->get_linker();
    VkPipelineLayoutCreateInfo pipeline_layout_create_info;
    setz(pipeline_layout_create_info);
    pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_create_info.setLayoutCount = 1;
    pipeline_layout_create_info.pSetLayouts = &(desset->get_layout());
    VKC(l->vkCreatePipelineLayout(dev->get_vulkan_data(),
        &pipeline_layout_create_info, nullptr,
        &vulkan_data));
}

gearoenix::render::pipeline::Layout::~Layout()
{
    const device::Physical* p = dev->get_physical_device();
    const Linker* l = p->get_instance()->get_linker();
    l->vkDestroyPipelineLayout(dev->get_vulkan_data(), vulkan_data,
        nullptr);
}

const VkPipelineLayout& gearoenix::render::pipeline::Layout::get_vulkan_data()
    const
{
    return vulkan_data;
}

const std::shared_ptr<gearoenix::render::descriptor::Set>& gearoenix::render::pipeline::Layout::get_descriptor_set() const
{
    return desset;
}

const gearoenix::render::device::Logical* gearoenix::render::pipeline::Layout::get_device() const
{
    return dev;
}
