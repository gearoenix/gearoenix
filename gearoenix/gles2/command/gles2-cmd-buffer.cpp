#include "gles2-cmd-buffer.hpp"

gearoenix::gles2::command::Buffer::~Buffer() {}

void gearoenix::gles2::command::Buffer::begin()
{
	render::command::Buffer::begin();
}

void gearoenix::gles2::command::Buffer::end()
{
	render::command::Buffer::end();
}

void gearoenix::gles2::command::Buffer::record(const std::shared_ptr<render::command::Buffer>& o)
{
    render::command::Buffer::record(o);
}

void gearoenix::gles2::command::Buffer::bind(const std::shared_ptr<render::pipeline::ResourceSet> &r)
{
	render::command::Buffer::bind(r);
}