#include "gx-vk-des-set.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../gx-vk-check.hpp"
#include "gx-vk-des-pool-manager.hpp"
#include "gx-vk-des-pool.hpp"

gearoenix::vulkan::descriptor::Set::Set(
    Pool* pool,
    std::shared_ptr<PoolManager> pool_manager,
    VkDescriptorSet vulkan_data) noexcept
    : vulkan_data(vulkan_data)
    , pool(pool)
    , pool_manager(std::move(pool_manager))
{
}

std::shared_ptr<gearoenix::vulkan::descriptor::Set> gearoenix::vulkan::descriptor::Set::construct(
    Pool* pool,
    std::shared_ptr<PoolManager> pool_manager,
    VkDescriptorSetLayout layout) noexcept
{
    VkDescriptorSet vulkan_data = nullptr;
    VkDescriptorSetAllocateInfo info;
    GX_SET_ZERO(info)
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    info.descriptorPool = pool->get_vulkan_data();
    info.descriptorSetCount = 1;
    info.pSetLayouts = &layout;
    const auto state = vkAllocateDescriptorSets(pool->get_logical_device().get_vulkan_data(), &info, &vulkan_data);
    if (VK_SUCCESS == state)
        return std::shared_ptr<Set>(new Set(pool, std::move(pool_manager), vulkan_data));
    else if (VK_ERROR_OUT_OF_POOL_MEMORY == state)
        return nullptr;
    GX_LOG_F("Error in descriptor set allocation: " << result_to_string(state))
}
//
//gearoenix::render::descriptor::Set::Set(Pool* pool,
//    SetLayout* lay,
//    const VkDescriptorBufferInfo& buff_info,
//    const VkDescriptorImageInfo& img_info)
//    : pool(pool)
//{
//    const device::Logical* d = pool->get_logical_device();
//    const VkDevice vkdev = d->get_vulkan_data();
//    const Linker* l = d->get_physical_device()->get_instance()->get_linker();
//    VkDescriptorSetAllocateInfo alloc_info;
//    setz(alloc_info);
//    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
//    alloc_info.descriptorPool = pool->get_vulkan_data();
//    alloc_info.descriptorSetCount = 1;
//    alloc_info.pSetLayouts = &(lay->get_vulkan_data());
//    VKC(l->vkAllocateDescriptorSets(vkdev, &alloc_info, &vulkan_data));
//    VkWriteDescriptorSet write_descriptor_sets[2];
//    setz(write_descriptor_sets[0]);
//    write_descriptor_sets[0].descriptorCount = 1;
//    write_descriptor_sets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//    write_descriptor_sets[0].dstBinding = 0;
//    write_descriptor_sets[0].dstSet = vulkan_data;
//    write_descriptor_sets[0].pBufferInfo = &buff_info;
//    write_descriptor_sets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//    setz(write_descriptor_sets[1]);
//    write_descriptor_sets[1].descriptorCount = 1;
//    write_descriptor_sets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//    write_descriptor_sets[1].dstBinding = 1;
//    write_descriptor_sets[1].dstSet = vulkan_data;
//    write_descriptor_sets[1].pImageInfo = &img_info;
//    write_descriptor_sets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//    l->vkUpdateDescriptorSets(vkdev, countof(write_descriptor_sets), write_descriptor_sets, 0, nullptr);
//}

gearoenix::vulkan::descriptor::Set::~Set() noexcept
{
    GX_VK_CHK(vkFreeDescriptorSets(
        pool->get_logical_device().get_vulkan_data(), pool->get_vulkan_data(), 1, &vulkan_data))
    pool_manager->pool_freed(pool);
}
//
//const VkDescriptorSet& gearoenix::render::descriptor::Set::get_vulkan_data() const
//{
//    return vulkan_data;
//}
//
//const VkDescriptorSetLayout& gearoenix::render::descriptor::Set::get_layout() const
//{
//    return layout;
//}
//
//const gearoenix::render::descriptor::Pool* gearoenix::render::descriptor::Set::get_pool() const
//{
//    return pool;
//}
//
//gearoenix::render::descriptor::Pool* gearoenix::render::descriptor::Set::get_pool()
//{
//    return pool;
//}
//
//void gearoenix::render::descriptor::Set::bind(pipeline::Pipeline* p, command::Buffer* c)
//{
//    c->bind_descriptor_set(
//        VK_PIPELINE_BIND_POINT_GRAPHICS,
//        p->get_layout()->get_vulkan_data(),
//        0,
//        vulkan_data);
//}
#endif
