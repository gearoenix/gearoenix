#ifndef GEAROENIX_VULKAN_COMMAND_MANAGER_HPP
#define GEAROENIX_VULKAN_COMMAND_MANAGER_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../../core/gx-cr-static.hpp"
#include "../../render/command/gx-rnd-cmd-manager.hpp"
#include <map>
#include <thread>

namespace gearoenix::vulkan::device {
class Logical;
}

namespace gearoenix::vulkan::command {
class Pool;
class Manager final : public render::command::Manager {
    GX_GET_REFC_PRV(std::shared_ptr<device::Logical>, logical_device)
private:
    std::map<std::size_t, std::shared_ptr<Pool>> indexed_pools;
    std::map<std::thread::id, std::shared_ptr<Pool>> threads_pools;

public:
    explicit Manager(std::shared_ptr<device::Logical> logical_device) noexcept;
    ~Manager() noexcept final;
    [[nodiscard]] render::command::Buffer* create_command_buffer(
        render::command::Type command_buffer_type,
        std::optional<std::size_t> thread_id) noexcept final;
};
}
#endif
#endif
