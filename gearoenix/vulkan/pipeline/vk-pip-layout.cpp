#include "vk-pip-layout.hpp"
#include "../device/vk-dev-logical.hpp"
#include "../device/vk-dev-physical.hpp"
#include "../vk-instance.hpp"
#include "../../core/cr-static.hpp"
#include "../vk-check.hpp"
gearoenix::render::pipeline::Layout::Layout(const std::shared_ptr<device::Logical> &logical_device):
    logical_device(logical_device) {
    auto &l = logical_device->get_physical_device()->get_instance()->get_linker();
    VkDescriptorSetLayoutBinding layout_binding;
    setz(layout_binding);
    layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layout_binding.descriptorCount = 1;
    layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    VkDescriptorSetLayoutCreateInfo descriptor_layout;
    setz(descriptor_layout);
    descriptor_layout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptor_layout.bindingCount = 1;
    descriptor_layout.pBindings = &layout_binding;
    VKC(l->vkCreateDescriptorSetLayout(logical_device->get_vulkan_data(), &descriptor_layout, nullptr, &descriptor_set_layout));
    VkPipelineLayoutCreateInfo pipeline_layout_create_info;
    setz(pipeline_layout_create_info);
    pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_create_info.setLayoutCount = 1;
    pipeline_layout_create_info.pSetLayouts = &descriptor_set_layout;
    VKC(l->vkCreatePipelineLayout(logical_device->get_vulkan_data(), &pipeline_layout_create_info, nullptr, &vulkan_data));
}

gearoenix::render::pipeline::Layout::~Layout() {
    auto &l = logical_device->get_physical_device()->get_instance()->get_linker();
    l->vkDestroyPipelineLayout(logical_device->get_vulkan_data(), vulkan_data, nullptr);
    l->vkDestroyDescriptorSetLayout(logical_device->get_vulkan_data(), descriptor_set_layout, nullptr);
}


const VkPipelineLayout &gearoenix::render::pipeline::Layout::get_vulkan_data() const {
    return vulkan_data;
}


const VkDescriptorSetLayout &gearoenix::render::pipeline::Layout::get_descriptor_set_layout() const {
    return descriptor_set_layout;
}
