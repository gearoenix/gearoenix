#include "gx-vk-cmd-manager.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../engine/gx-vk-eng-engine.hpp"

gearoenix::vulkan::command::Manager::Manager(const engine::Engine& e) noexcept
    : e(e)
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
            indexed_pools.emplace(*thread_index, Pool(e.get_logical_device()));
            search = indexed_pools.find(*thread_index);
        }
        pool = &(search->second);
    } else {
        const auto id = std::this_thread::get_id();
        auto search = threads_pools.find(id);
        if (threads_pools.end() == search) {
            threads_pools.emplace(id, Pool(e.get_logical_device()));
            search = threads_pools.find(id);
        }
        pool = &(search->second);
    }
    return Buffer(pool, t);
}

std::vector<std::shared_ptr<gearoenix::vulkan::command::Buffer>> gearoenix::vulkan::command::Manager::create_frame_based() noexcept
{
    std::vector<std::shared_ptr<Buffer>> result(e.get_swapchain().get_image_views().size());
    for (auto& c : result)
        c = std::make_shared<Buffer>(std::move(create(Type::Primary)));
    return result;
}

#endif