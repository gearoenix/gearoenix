#include "vk-des-set.hpp"
#include "../../core/cr-static.hpp"
#include "../../render/shader/rnd-shd-shader.hpp"
#include "../device/vk-dev-logical.hpp"
#include "../device/vk-dev-physical.hpp"
#include "../pipeline/vk-pip-layout.hpp"
#include "../vk-check.hpp"
#include "../vk-instance.hpp"
#include "vk-des-pool.hpp"

gearoenix::render::descriptor::Set::Set(
    Pool* pool,
    const VkDescriptorBufferInfo& buff_info)
    : pool(pool)
{
    const device::Logical* d = pool->get_logical_device();
    const VkDevice vkdev = d->get_vulkan_data();
    const Linker* l = d->get_physical_device()->get_instance()->get_linker();
    VkDescriptorSetLayoutBinding layout_binding;
    setz(layout_binding);
    layout_binding.descriptorCount = 1;
    layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    VkDescriptorSetLayoutCreateInfo descriptor_layout;
    setz(descriptor_layout);
    descriptor_layout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptor_layout.bindingCount = 1;
    descriptor_layout.pBindings = &layout_binding;
    VKC(l->vkCreateDescriptorSetLayout(vkdev, &descriptor_layout, nullptr, &layout));
    VkDescriptorSetAllocateInfo alloc_info;
    setz(alloc_info);
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = pool->get_vulkan_data();
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &layout;
    VKC(l->vkAllocateDescriptorSets(vkdev, &alloc_info, &vulkan_data));
    VkWriteDescriptorSet write_descriptor_set;
    setz(write_descriptor_set);
    write_descriptor_set.descriptorCount = 1;
    write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write_descriptor_set.dstBinding = 0;
    write_descriptor_set.dstSet = vulkan_data;
    write_descriptor_set.pBufferInfo = &buff_info;
    write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    l->vkUpdateDescriptorSets(vkdev, 1, &write_descriptor_set, 0, nullptr);
}

gearoenix::render::descriptor::Set::Set(
    Pool* pool,
    const VkDescriptorBufferInfo& buff_info,
    const VkDescriptorImageInfo& img_info)
    : pool(pool)
{
    const device::Logical* d = pool->get_logical_device();
    const VkDevice vkdev = d->get_vulkan_data();
    const Linker* l = d->get_physical_device()->get_instance()->get_linker();
    VkDescriptorSetLayoutBinding layout_bindings[2];
    setz(layout_bindings[0]);
    layout_bindings[0].binding = 0;
    layout_bindings[0].descriptorCount = 1;
    layout_bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layout_bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    setz(layout_bindings[1]);
    layout_bindings[1].binding = 1;
    layout_bindings[1].descriptorCount = 1;
    layout_bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layout_bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    VkDescriptorSetLayoutCreateInfo descriptor_layout;
    setz(descriptor_layout);
    descriptor_layout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptor_layout.bindingCount = countof(layout_bindings);
    descriptor_layout.pBindings = layout_bindings;
    VKC(l->vkCreateDescriptorSetLayout(vkdev, &descriptor_layout, nullptr, &layout));
    VkDescriptorSetAllocateInfo alloc_info;
    setz(alloc_info);
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = pool->get_vulkan_data();
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &layout;
    VKC(l->vkAllocateDescriptorSets(vkdev, &alloc_info, &vulkan_data));
    VkWriteDescriptorSet write_descriptor_sets[2];
    setz(write_descriptor_sets[0]);
    write_descriptor_sets[0].descriptorCount = 1;
    write_descriptor_sets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write_descriptor_sets[0].dstBinding = 0;
    write_descriptor_sets[0].dstSet = vulkan_data;
    write_descriptor_sets[0].pBufferInfo = &buff_info;
    write_descriptor_sets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    setz(write_descriptor_sets[1]);
    write_descriptor_sets[1].descriptorCount = 1;
    write_descriptor_sets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write_descriptor_sets[1].dstBinding = 1;
    write_descriptor_sets[1].dstSet = vulkan_data;
    write_descriptor_sets[1].pImageInfo = &img_info;
    write_descriptor_sets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    l->vkUpdateDescriptorSets(vkdev, countof(write_descriptor_sets), write_descriptor_sets, 0, nullptr);
}

gearoenix::render::descriptor::Set::~Set() {}

const VkDescriptorSet& gearoenix::render::descriptor::Set::get_vulkan_data() const
{
    return vulkan_data;
}

const VkDescriptorSetLayout& gearoenix::render::descriptor::Set::get_layout() const
{
    return layout;
}

const gearoenix::render::descriptor::Pool* gearoenix::render::descriptor::Set::get_pool() const
{
    return pool;
}

gearoenix::render::descriptor::Pool* gearoenix::render::descriptor::Set::get_pool()
{
    return pool;
}
