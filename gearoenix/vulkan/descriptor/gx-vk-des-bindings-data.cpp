#include "gx-vk-des-bindings-data.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../device/gx-vk-dev-logical.hpp"
#include "gx-vk-des-pool-manager.hpp"
#include "gx-vk-des-set-layout.hpp"

std::vector<VkDescriptorPoolSize> gearoenix::vulkan::descriptor::BindingsData::create_pool_sizes(
    const std::vector<VkDescriptorSetLayoutBinding>& data)
{
    std::map<VkDescriptorType, std::uint32_t> szs;
    for (const auto& b : data) {
        szs[b.descriptorType] += b.descriptorCount;
    }
    std::vector<VkDescriptorPoolSize> result(szs.size());
    std::size_t i = 0;
    for (const auto& [t, c] : szs) {
        auto& r = result[i];
        r.descriptorCount = c;
        r.type = t;
        ++i;
    }
    return result;
}

gearoenix::vulkan::descriptor::BindingsData::BindingsData(
    const device::Logical& logical_device,
    const std::vector<VkDescriptorSetLayoutBinding>& data)
    : logical_device(logical_device)
    , layout(new SetLayout(logical_device, data))
    , pool_sizes(create_pool_sizes(data))
{
}

std::shared_ptr<gearoenix::vulkan::descriptor::Set> gearoenix::vulkan::descriptor::BindingsData::create_set(
    const std::optional<std::size_t> kernel_index)
{
    std::variant<std::size_t, std::thread::id> ti;
    if (kernel_index.has_value())
        ti = *kernel_index;
    else
        ti = std::this_thread::get_id();
    std::shared_ptr<PoolManager> pool = pools[ti].lock();
    if (nullptr == pool) {
        pool = PoolManager::construct(logical_device);
        pools[ti] = pool;
    }
    return pool->create_set(pool_sizes, *layout);
}

gearoenix::vulkan::descriptor::BindingsData::~BindingsData() = default;

#endif