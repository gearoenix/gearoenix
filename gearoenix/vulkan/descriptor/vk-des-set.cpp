#include "vk-des-set.hpp"
#include "../../core/cr-static.hpp"
#include "../../render/shader/render-shader.hpp"
#include "../device/vk-dev-logical.hpp"
#include "../device/vk-dev-physical.hpp"
#include "../pipeline/vk-pip-layout.hpp"
#include "../vk-check.hpp"
#include "../vk-instance.hpp"
#include "vk-des-pool.hpp"

gearoenix::render::descriptor::Set::Set(
    uint64_t shader_id,
    Pool* pool,
    const VkDescriptorBufferInfo& buff_info)
    : pool(pool)
{
    const device::Logical* d = pool->get_logical_device();
    const VkDevice vkdev = d->get_vulkan_data();
    const Linker* l = d->get_physical_device()->get_instance()->get_linker();
    const std::vector<shader::Shader::ResourceDescription> shader_resources = shader::Shader::get_resources_descriptions(
        static_cast<shader::Shader::IdType>(shader_id));
    unsigned int shader_resources_size = shader_resources.size();
    std::vector<VkDescriptorSetLayoutBinding> layout_bindings(shader_resources_size);
    for (unsigned int i = 0; i < shader_resources_size; ++i) {
        setz(layout_bindings[i]);
        switch (shader_resources[i].t) {
        case shader::Shader::ResourceDescription::UNIFORM:
            layout_bindings[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            break;
        default:
            LOGF("Unexpected");
        }
        layout_bindings[i].descriptorCount = shader_resources[i].count;
        layout_bindings[i].stageFlags = 0;
        for (const shader::Shader::ResourceDescription::BindingStage& binding_stage :
            shader_resources[i].binding_stages) {
            switch (binding_stage) {
            case shader::Shader::ResourceDescription::VERTEX:
                layout_bindings[i].stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
                break;
            case shader::Shader::ResourceDescription::FRAGMENT:
                layout_bindings[i].stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
                break;
            default:
                LOGF("Unexpected");
                break;
            }
        }
    }
    VkDescriptorSetLayoutCreateInfo descriptor_layout;
    setz(descriptor_layout);
    descriptor_layout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptor_layout.bindingCount = layout_bindings.size();
    descriptor_layout.pBindings = layout_bindings.data();
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
    write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_descriptor_set.dstSet = vulkan_data;
    write_descriptor_set.descriptorCount = 1;
    write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write_descriptor_set.pBufferInfo = &buff_info;
    write_descriptor_set.dstBinding = 0;
    l->vkUpdateDescriptorSets(vkdev, 1, &write_descriptor_set, 0, nullptr);
}

gearoenix::render::descriptor::Set::~Set() {}

const VkDescriptorSet& gearoenix::render::descriptor::Set::get_vulkan_data() const
{
    return vulkan_data;
}
