#include "gx-vk-cmd-manager.hpp"
#ifdef GX_USE_VULKAN
#include "../device/gx-vk-dev-logical.hpp"
#include "gx-vk-cmd-buffer.hpp"
#include "gx-vk-cmd-pool.hpp"

gearoenix::vulkan::command::Manager::Manager(std::shared_ptr<device::Logical> logical_device) noexcept
    : logical_device(std::move(logical_device))
{
}

gearoenix::vulkan::command::Manager::~Manager() noexcept
{
    indexed_pools.clear();
    threads_pools.clear();
}

gearoenix::render::command::Buffer* gearoenix::vulkan::command::Manager::create_command_buffer(
    const render::command::Type command_buffer_type,
    const std::optional<std::size_t> thread_id) noexcept
{
    auto& pool = thread_id.has_value() ? indexed_pools[*thread_id] : threads_pools[std::this_thread::get_id()];
    if (nullptr == pool)
        pool = std::make_shared<Pool>(logical_device);
    return new Buffer(pool, command_buffer_type);
}

#endif