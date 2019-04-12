#include "gles2-cmd-manager.hpp"
#include "../../system/sys-log.hpp"
#include "gles2-cmd-buffer.hpp"

gearoenix::render::command::Buffer * gearoenix::gles2::command::Manager::create_primary_command_buffer()
{
	return new Buffer();
}

gearoenix::render::command::Buffer * gearoenix::gles2::command::Manager::create_secondary_command_buffer()
{
	return new Buffer();
}

gearoenix::render::command::Buffer * gearoenix::gles2::command::Manager::create_primary_command_buffer(const unsigned int)
{
	return new Buffer();
}

gearoenix::render::command::Buffer * gearoenix::gles2::command::Manager::create_secondary_command_buffer(const unsigned int)
{
	return new Buffer();
}
