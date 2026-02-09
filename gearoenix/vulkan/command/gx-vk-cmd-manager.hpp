#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../core/gx-cr-singleton.hpp"
#include "gx-vk-cmd-buffer.hpp"
#include "gx-vk-cmd-pool.hpp"
#include "gx-vk-cmd-type.hpp"

#include <boost/container/flat_map.hpp>

#include <memory>
#include <optional>
#include <thread>

namespace gearoenix::vulkan::command {
struct Manager final: core::Singleton<Manager> {
private:
    std::mutex this_lock;
    boost::container::flat_map<std::uint64_t, PoolPtr> indexed_pools;
    boost::container::flat_map<std::thread::id, PoolPtr> threads_pools;

public:
    Manager();
    Manager(Manager&&) = delete;
    Manager(const Manager&) = delete;
    Manager& operator=(Manager&&) = delete;
    Manager& operator=(const Manager&) = delete;
    ~Manager() override;

    [[nodiscard]] std::shared_ptr<Buffer> create(Type buffer_type = Type::Primary, std::optional<std::uint64_t> thread_index = std::nullopt);

    /// The created command buffer will have its own dedicated pool so it can be passed between threads.
    [[nodiscard]] static std::shared_ptr<Buffer> create_thread_independent();
};
}
#endif