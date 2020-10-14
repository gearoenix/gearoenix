#ifndef GEAROENIX_RENDER_COMMAND_MANAGER_HPP
#define GEAROENIX_RENDER_COMMAND_MANAGER_HPP
#include "gx-rnd-cmd-type.hpp"
#include <optional>

namespace gearoenix::render::command {
class Buffer;
/// All of the functions must be internally synchronized for each command pool
class Manager {
public:
    virtual ~Manager() noexcept = default;
    [[nodiscard]] virtual Buffer* create_command_buffer(
        Type command_buffer_type,
        std::optional<std::size_t> thread_id) noexcept = 0;
    [[nodiscard]] Buffer* create_primary_command_buffer(
        std::optional<std::size_t> thread_id = std::nullopt) noexcept;
    [[nodiscard]] Buffer* create_secondary_command_buffer(
        std::optional<std::size_t> thread_id = std::nullopt) noexcept;
};
}

#endif