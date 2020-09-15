#include "gx-gles2-cmd-manager.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../system/gx-sys-log.hpp"
#include "gx-gles2-cmd-buffer.hpp"

gearoenix::render::command::Buffer* gearoenix::gles2::command::Manager::create_primary_command_buffer() noexcept
{
    return new Buffer();
}

gearoenix::render::command::Buffer* gearoenix::gles2::command::Manager::create_secondary_command_buffer() noexcept
{
    return new Buffer();
}

gearoenix::render::command::Buffer* gearoenix::gles2::command::Manager::create_primary_command_buffer(const unsigned int) noexcept
{
    return new Buffer();
}

gearoenix::render::command::Buffer* gearoenix::gles2::command::Manager::create_secondary_command_buffer(const unsigned int) noexcept
{
    return new Buffer();
}
#endif