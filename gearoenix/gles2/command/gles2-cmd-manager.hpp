#ifndef GEAROENIX_GLES2_COMMAND_MANAGER_HPP
#define GEAROENIX_GLES2_COMMAND_MANAGER_HPP
#include "../../render/command/rnd-cmd-manager.hpp"
namespace gearoenix::gles2::command {
        class Manager : public render::command::Manager {
        public:
            render::command::Buffer* create_primary_command_buffer() final;
            render::command::Buffer* create_secondary_command_buffer() final;
            render::command::Buffer* create_primary_command_buffer(unsigned int kernel_index) final;
            render::command::Buffer* create_secondary_command_buffer(unsigned int kernel_index) final;
        };
    }
#endif