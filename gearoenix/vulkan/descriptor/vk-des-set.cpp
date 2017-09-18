//#include "vk-des-set.hpp"
//#include "../../core/cr-static.hpp"
//#include "../buffer/vk-buf-uniform.hpp"
//#include "../device/vk-dev-logical.hpp"
//#include "../device/vk-dev-physical.hpp"
//#include "../pipeline/vk-pip-layout.hpp"
//#include "../vk-check.hpp"
//#include "../vk-instance.hpp"
//#include "vk-des-pool.hpp"

//gearoenix::render::descriptor::Set::Set(
//    const std::shared_ptr<Pool>& pool,
//    const std::shared_ptr<pipeline::Layout>& pipeline_layout,
//    const std::shared_ptr<buffer::Uniform>& uniform)
//    : pool(pool)
//    , pipeline_layout(pipeline_layout)
//    , uniform(uniform)
//{
//    auto d = pool->get_logical_device();
//    auto l = d->get_physical_device()->get_instance()->get_linker();
//    VkDescriptorSetAllocateInfo alloc_info;
//    setz(alloc_info);
//    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
//    alloc_info.descriptorPool = pool->get_vulkan_data();
//    alloc_info.descriptorSetCount = 1;
//    alloc_info.pSetLayouts = &(pipeline_layout->get_descriptor_set_layout());
//    VKC(l->vkAllocateDescriptorSets(d->get_vulkan_data(), &alloc_info,
//        &vulkan_data));
//    VkDescriptorBufferInfo uniform_des;
//    setz(uniform_des);
//    uniform_des.buffer = uniform->get_vulkan_data();
//    uniform_des.range = uniform->get_data_size();
//    VkWriteDescriptorSet write_descriptor_set;
//    setz(write_descriptor_set);
//    write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//    write_descriptor_set.dstSet = vulkan_data;
//    write_descriptor_set.descriptorCount = 1;
//    write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//    write_descriptor_set.pBufferInfo = &(uniform_des);
//    write_descriptor_set.dstBinding = 0;
//    l->vkUpdateDescriptorSets(d->get_vulkan_data(), 1, &write_descriptor_set, 0,
//        nullptr);
//}

//gearoenix::render::descriptor::Set::~Set() {}

//const VkDescriptorSet& gearoenix::render::descriptor::Set::get_vulkan_data()
//    const
//{
//    return vulkan_data;
//}
