#include "glc3-cmd-manager.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../system/sys-log.hpp"
#include "glc3-cmd-buffer.hpp"

gearoenix::render::command::Buffer* gearoenix::glc3::command::Manager::create_primary_command_buffer() noexcept
{
    return new Buffer();
}

gearoenix::render::command::Buffer* gearoenix::glc3::command::Manager::create_secondary_command_buffer() noexcept
{
    return new Buffer();
}

gearoenix::render::command::Buffer* gearoenix::glc3::command::Manager::create_primary_command_buffer(const unsigned int) noexcept
{
    return new Buffer();
}

gearoenix::render::command::Buffer* gearoenix::glc3::command::Manager::create_secondary_command_buffer(const unsigned int) noexcept
{
    return new Buffer();
}
#endif