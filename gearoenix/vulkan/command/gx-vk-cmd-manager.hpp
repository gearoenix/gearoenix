#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-vk-cmd-buffer.hpp"
#include "gx-vk-cmd-pool.hpp"
#include "gx-vk-cmd-type.hpp"
#include <map>
#include <memory>
#include <optional>
#include <thread>

namespace gearoenix::vulkan::engine {
struct Engine;
}

namespace gearoenix::vulkan::command {
struct Manager final {
    GX_GET_CRRF_PRV(engine::Engine, e);

    std::mutex this_lock;
    std::map<std::uint64_t, Pool> indexed_pools;
    std::map<std::thread::id, Pool> threads_pools;

public:
    Manager(const Manager&) = delete;
    Manager(Manager&&) = delete;
    explicit Manager(const engine::Engine& e);
    ~Manager();
    Manager& operator=(const Manager&) = delete;
    Manager& operator=(Manager&&) = delete;
    [[nodiscard]] Buffer create(Type buffer_type, std::optional<std::uint64_t> thread_index = std::nullopt);
    [[nodiscard]] std::vector<std::shared_ptr<Buffer>> create_frame_based();
};
}
#endif