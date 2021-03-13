#include "gx-vk-des-pool-manager.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../platform/gx-plt-log.hpp"
#include "gx-vk-des-pool.hpp"
#include "gx-vk-des-set-layout.hpp"
#include "gx-vk-des-set.hpp"

gearoenix::vulkan::descriptor::PoolManager::PoolManager(const device::Logical& logical_device) noexcept
    : logical_device(logical_device)
{
}

std::shared_ptr<gearoenix::vulkan::descriptor::PoolManager> gearoenix::vulkan::descriptor::PoolManager::construct(
    const device::Logical& logical_device) noexcept
{
    std::shared_ptr<PoolManager> result(new PoolManager(logical_device));
    result->self = result;
    return result;
}

std::shared_ptr<gearoenix::vulkan::descriptor::Set> gearoenix::vulkan::descriptor::PoolManager::create_set(
    const std::vector<VkDescriptorPoolSize>& pool_sizes,
    const SetLayout& layout) noexcept
{
    auto s = self.lock();
    for (auto iter = free_pools.begin(); iter != free_pools.end();) {
        auto result = Set::construct(*iter, s, layout.get_vulkan_data());
        if (result != nullptr)
            return result;
        iter = free_pools.erase(iter);
    }
    auto* pool = new Pool(logical_device, pool_sizes.data(), static_cast<std::uint32_t>(pool_sizes.size()));
    pools.emplace_back(pool);
    free_pools.emplace(pool);
    auto result = Set::construct(pool, s, layout.get_vulkan_data());
    if (result != nullptr)
        return result;
    GX_UNEXPECTED
}

void gearoenix::vulkan::descriptor::PoolManager::pool_freed(Pool* const p) noexcept
{
    GX_GUARD_LOCK(this)
    free_pools.emplace(p);
}

gearoenix::vulkan::descriptor::PoolManager::~PoolManager() noexcept = default;

#endif
