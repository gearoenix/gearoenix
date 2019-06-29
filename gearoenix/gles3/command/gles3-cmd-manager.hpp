#ifndef GEAROENIX_GLES3_COMMAND_MANAGER_HPP
#define GEAROENIX_GLES3_COMMAND_MANAGER_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES3
#include "../../render/command/rnd-cmd-manager.hpp"
namespace gearoenix::gles3::command {
class Manager : public render::command::Manager {
public:
    render::command::Buffer* create_primary_command_buffer() final;
    render::command::Buffer* create_secondary_command_buffer() final;
    render::command::Buffer* create_primary_command_buffer(unsigned int kernel_index) final;
    render::command::Buffer* create_secondary_command_buffer(unsigned int kernel_index) final;
};
}
#endif
#endif