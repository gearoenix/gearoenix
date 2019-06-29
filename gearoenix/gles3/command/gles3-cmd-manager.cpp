#include "gles3-cmd-manager.hpp"
#ifdef GX_USE_OPENGL_ES3
#include "../../system/sys-log.hpp"
#include "gles3-cmd-buffer.hpp"

gearoenix::render::command::Buffer* gearoenix::gles3::command::Manager::create_primary_command_buffer()
{
    return new Buffer();
}

gearoenix::render::command::Buffer* gearoenix::gles3::command::Manager::create_secondary_command_buffer()
{
    return new Buffer();
}

gearoenix::render::command::Buffer* gearoenix::gles3::command::Manager::create_primary_command_buffer(const unsigned int)
{
    return new Buffer();
}

gearoenix::render::command::Buffer* gearoenix::gles3::command::Manager::create_secondary_command_buffer(const unsigned int)
{
    return new Buffer();
}
#endif