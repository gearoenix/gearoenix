#ifndef GEAROENIX_GLES2_COMMAND_MANAGER_HPP
#define GEAROENIX_GLES2_COMMAND_MANAGER_HPP
#include "../../render/command/rnd-cmd-manager.hpp"
namespace gearoenix {
namespace gles2 {
    namespace command {
        class Manager : public render::command::Manager {
        public:
            render::command::Buffer* create_primary_command_buffer() override final;
            render::command::Buffer* create_secondary_command_buffer() override final;
            render::command::Buffer* create_primary_command_buffer(const unsigned int kernel_index) override final;
            render::command::Buffer* create_secondary_command_buffer(const unsigned int kernel_index) override final;
        };
    }
}
}
#endif