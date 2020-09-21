#include "gx-gles2-cmd-manager.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../system/gx-sys-log.hpp"
#include "gx-gles2-cmd-buffer.hpp"

gearoenix::render::command::Buffer* gearoenix::gles2::command::Manager::create_command_buffer(
    render::command::Type command_buffer_type,
    std::optional<std::size_t>) noexcept
{
    return new Buffer(command_buffer_type);
}
#endif