#include "gx-vk-des-set-layout.hpp"
#ifdef USE_VULKAN
#include "../../core/gx-cr-static.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../gx-vk-check.hpp"
#include "../gx-vk-instance.hpp"
#include "gx-vk-des-pool.hpp"

gearoenix::render::descriptor::SetLayout::SetLayout(
    device::Logical* dev,
    shader::Id shader_id)
    : dev(dev)
{
    const VkDevice vkdev = dev->get_vulkan_data();
    const Linker* l = dev->get_physical_device()->get_instance()->get_linker();
    const std::vector<shader::ResourceDescription> shader_resources = shader::Shader::get_resources_descriptions(
        static_cast<shader::Id>(shader_id));
    unsigned int shader_resources_size = shader_resources.size();
    std::vector<VkDescriptorSetLayoutBinding> layout_bindings(shader_resources_size);
    for (unsigned int i = 0; i < shader_resources_size; ++i) {
        setz(layout_bindings[i]);
        layout_bindings[i].binding = i;
        switch (shader_resources[i].t) {
        case shader::ResourceDescription::UNIFORM:
            layout_bindings[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            break;
        case shader::ResourceDescription::SAMPER2D:
            layout_bindings[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            break;
        default:
            LOGF("Unexpected");
        }
        layout_bindings[i].descriptorCount = shader_resources[i].count;
        layout_bindings[i].stageFlags = 0;
        for (const shader::stage::Id& binding_stage :
            shader_resources[i].binding_stages) {
            switch (binding_stage) {
            case shader::stage::VERTEX:
                layout_bindings[i].stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
                break;
            case shader::stage::FRAGMENT:
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
    VKC(l->vkCreateDescriptorSetLayout(vkdev, &descriptor_layout, nullptr, &vulkan_data));
}
gearoenix::render::descriptor::SetLayout::~SetLayout()
{
    const VkDevice vkdev = dev->get_vulkan_data();
    const Linker* l = dev->get_physical_device()->get_instance()->get_linker();
    l->vkDestroyDescriptorSetLayout(vkdev, vulkan_data, nullptr);
}

const gearoenix::render::device::Logical* gearoenix::render::descriptor::SetLayout::get_logical_device() const
{
    return dev;
}

const VkDescriptorSetLayout& gearoenix::render::descriptor::SetLayout::get_vulkan_data() const
{
    return vulkan_data;
}
#endif
