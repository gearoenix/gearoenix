#include "gx-vk-cmd-manager.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-swapchain.hpp"

gearoenix::vulkan::command::Manager::Manager()
    : Singleton(this)
{
}

gearoenix::vulkan::command::Manager::~Manager() = default;

gearoenix::vulkan::command::Buffer gearoenix::vulkan::command::Manager::create(const Type buffer_type, const std::optional<std::uint64_t> thread_index)
{
    const std::lock_guard _lg(this_lock);
    std::shared_ptr<Pool> pool;
    if (thread_index.has_value()) {
        auto search = indexed_pools.find(*thread_index);
        if (indexed_pools.end() == search) {
            search = indexed_pools.emplace(*thread_index, std::make_shared<Pool>()).first;
        }
        pool = search->second;
    } else {
        const auto id = std::this_thread::get_id();
        auto search = threads_pools.find(id);
        if (threads_pools.end() == search) {
            search = threads_pools.emplace(id, std::make_shared<Pool>()).first;
        }
        pool = search->second;
    }
    return Buffer(std::move(pool), buffer_type);
}

std::vector<std::shared_ptr<gearoenix::vulkan::command::Buffer>> gearoenix::vulkan::command::Manager::create_frame_based()
{
    std::vector<std::shared_ptr<Buffer>> result(Swapchain::get().get_image_views().size());
    for (auto& c : result) {
        c = std::make_shared<Buffer>(std::move(create(Type::Primary)));
    }
    return result;
}

#endif