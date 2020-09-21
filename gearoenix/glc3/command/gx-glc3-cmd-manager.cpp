#include "gx-glc3-cmd-manager.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "gx-glc3-cmd-buffer.hpp"

gearoenix::render::command::Buffer* gearoenix::glc3::command::Manager::create_command_buffer(
    const render::command::Type command_buffer_type,
    const std::optional<std::size_t>) noexcept
{
    return new Buffer(command_buffer_type);
}

#endif
