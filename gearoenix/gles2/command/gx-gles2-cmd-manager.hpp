#ifndef GEAROENIX_GLES2_COMMAND_MANAGER_HPP
#define GEAROENIX_GLES2_COMMAND_MANAGER_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../render/command/gx-rnd-cmd-manager.hpp"
namespace gearoenix::gles2::command {
class Manager : public render::command::Manager {
public:
    render::command::Buffer* create_primary_command_buffer() noexcept final;
    render::command::Buffer* create_secondary_command_buffer() noexcept final;
    render::command::Buffer* create_primary_command_buffer(unsigned int kernel_index) noexcept final;
    render::command::Buffer* create_secondary_command_buffer(unsigned int kernel_index) noexcept final;
};
}
#endif
#endif