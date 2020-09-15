#ifndef GEAROENIX_RENDER_COMMAND_MANAGER_HPP
#define GEAROENIX_RENDER_COMMAND_MANAGER_HPP

namespace gearoenix::render::command {
class Buffer;
/// All of the functions must be internally synchronized for each command pool
class Manager {
public:
    virtual ~Manager() noexcept = default;
    virtual Buffer* create_primary_command_buffer() noexcept = 0;
    virtual Buffer* create_secondary_command_buffer() noexcept = 0;
    virtual Buffer* create_primary_command_buffer(unsigned int kernel_index) noexcept = 0;
    virtual Buffer* create_secondary_command_buffer(unsigned int kernel_index) noexcept = 0;
};
}

#endif