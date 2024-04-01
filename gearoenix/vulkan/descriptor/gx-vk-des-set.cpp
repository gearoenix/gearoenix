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
    VkDescriptorSet vulkan_data)
    : vulkan_data(vulkan_data)
    , pool(pool)
    , pool_manager(std::move(pool_manager))
{
}

std::shared_ptr<gearoenix::vulkan::descriptor::Set> gearoenix::vulkan::descriptor::Set::construct(
    Pool* pool,
    std::shared_ptr<PoolManager> pool_manager,
    VkDescriptorSetLayout layout)
{
    VkDescriptorSet vulkan_data = nullptr;
    VkDescriptorSetAllocateInfo info;
    GX_SET_ZERO(info);
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    info.descriptorPool = pool->get_vulkan_data();
    info.descriptorSetCount = 1;
    info.pSetLayouts = &layout;
    const auto state = vkAllocateDescriptorSets(pool->get_logical_device().get_vulkan_data(), &info, &vulkan_data);
    if (VK_SUCCESS == state)
        return std::shared_ptr<Set>(new Set(pool, std::move(pool_manager), vulkan_data));
    else if (VK_ERROR_OUT_OF_POOL_MEMORY == state)
        return nullptr;
    GX_LOG_F("Error in descriptor set allocation: " << result_to_string(state));
}

gearoenix::vulkan::descriptor::Set::~Set()
{
    GX_VK_CHK(vkFreeDescriptorSets(
        pool->get_logical_device().get_vulkan_data(), pool->get_vulkan_data(), 1, &vulkan_data));
    pool_manager->pool_freed(pool);
}

void gearoenix::vulkan::descriptor::Set::write(
    const VkWriteDescriptorSet* const data,
    const std::uint32_t count)
{
    vkUpdateDescriptorSets(pool->get_logical_device().get_vulkan_data(), count, data, 0, nullptr);
}

#endif
