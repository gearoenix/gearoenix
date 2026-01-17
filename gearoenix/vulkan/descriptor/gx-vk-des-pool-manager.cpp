#include "gx-vk-des-pool-manager.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../platform/gx-plt-log.hpp"
#include "gx-vk-des-pool.hpp"
#include "gx-vk-des-set-layout.hpp"
#include "gx-vk-des-set.hpp"

gearoenix::vulkan::descriptor::PoolManager::PoolManager() = default;

std::shared_ptr<gearoenix::vulkan::descriptor::PoolManager> gearoenix::vulkan::descriptor::PoolManager::construct()
{
    std::shared_ptr<PoolManager> result(new PoolManager());
    result->self = result;
    return result;
}

std::shared_ptr<gearoenix::vulkan::descriptor::Set> gearoenix::vulkan::descriptor::PoolManager::create_set(
    const std::vector<VkDescriptorPoolSize>& pool_sizes, const SetLayout& layout)
{
    const auto s = self.lock();
    const std::lock_guard _lg(this_lock);
    for (auto iter = free_pools.begin(); iter != free_pools.end(); iter = free_pools.erase(iter)) {
        if (auto result = Set::construct(*iter, s, layout.get_vulkan_data()); result != nullptr) {
            return result;
        }
    }
    auto* const pool = new Pool(pool_sizes.data(), static_cast<std::uint32_t>(pool_sizes.size()));
    pools.emplace_back(pool);
    free_pools.emplace(pool);
    if (auto result = Set::construct(pool, s, layout.get_vulkan_data()); result != nullptr) {
        return result;
    }
    GX_UNEXPECTED;
}

void gearoenix::vulkan::descriptor::PoolManager::pool_freed(Pool* const p)
{
    const std::lock_guard _lg(this_lock);
    free_pools.emplace(p);
}

gearoenix::vulkan::descriptor::PoolManager::~PoolManager() = default;

#endif
