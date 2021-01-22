#ifndef GEAROENIX_VULKAN_COMMAND_MANAGER_HPP
#define GEAROENIX_VULKAN_COMMAND_MANAGER_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include <map>
#include <thread>

namespace gearoenix::vulkan::device {
struct Logical;
}

namespace gearoenix::vulkan::command {
struct Pool;
struct Manager final {
    GX_GET_REFC_PRV(std::shared_ptr<device::Logical>, logical_device)
private:
    std::map<std::size_t, std::shared_ptr<Pool>> indexed_pools;
    std::map<std::thread::id, std::shared_ptr<Pool>> threads_pools;

public:
    explicit Manager(std::shared_ptr<device::Logical> logical_device) noexcept;
    ~Manager() noexcept;
    [[nodiscard]] Buffer* create_command_buffer(
        render::command::Type command_buffer_type,
        std::optional<std::size_t> thread_id) noexcept;
};
}
#endif
#endif
