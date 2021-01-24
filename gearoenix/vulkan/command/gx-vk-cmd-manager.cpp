#include "gx-vk-cmd-manager.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../device/gx-vk-dev-logical.hpp"

gearoenix::vulkan::command::Manager::Manager(const device::Logical& logical_device) noexcept
    : logical_device(logical_device)
{
}

gearoenix::vulkan::command::Manager::~Manager() noexcept = default;

gearoenix::vulkan::command::Buffer gearoenix::vulkan::command::Manager::create(const Type t, const std::optional<std::size_t> thread_index) noexcept
{
    GX_GUARD_LOCK(this)
    Pool* pool;
    if (thread_index.has_value()) {
        auto search = indexed_pools.find(*thread_index);
        if (indexed_pools.end() == search) {
            indexed_pools.emplace(*thread_index, Pool(logical_device));
            search = indexed_pools.find(*thread_index);
        }
        pool = &(search->second);
    } else {
        const auto id = std::this_thread::get_id();
        auto search = threads_pools.find(id);
        if (threads_pools.end() == search) {
            threads_pools.emplace(id, Pool(logical_device));
            search = threads_pools.find(id);
        }
        pool = &(search->second);
    }
    return Buffer(pool, t);
}

#endif