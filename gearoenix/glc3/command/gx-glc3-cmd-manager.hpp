#ifndef GEAROENIX_GLC3_COMMAND_MANAGER_HPP
#define GEAROENIX_GLC3_COMMAND_MANAGER_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../render/command/gx-rnd-cmd-manager.hpp"

namespace gearoenix::glc3::command {
class Manager : public render::command::Manager {
public:
    [[nodiscard]] render::command::Buffer* create_command_buffer(
        render::command::Type command_buffer_type,
        std::optional<std::size_t> thread_id) noexcept final;
};
}
#endif
#endif