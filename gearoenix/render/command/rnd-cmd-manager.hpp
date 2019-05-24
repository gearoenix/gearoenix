#ifndef GEAROENIX_RENDER_COMMAND_MANAGER_HPP
#define GEAROENIX_RENDER_COMMAND_MANAGER_HPP

namespace gearoenix::render::command {
class Buffer;
/// All of the functions must be internally synchronized for each command pool
class Manager {
public:
    virtual Buffer* create_primary_command_buffer() = 0;
    virtual Buffer* create_secondary_command_buffer() = 0;
    virtual Buffer* create_primary_command_buffer(unsigned int kernel_index) = 0;
    virtual Buffer* create_secondary_command_buffer(unsigned int kernel_index) = 0;
};
}

#endif