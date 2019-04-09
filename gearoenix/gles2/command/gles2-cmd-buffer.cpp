#include "gles2-cmd-buffer.hpp"
#include "../../system/sys-log.hpp"
#include "../texture/gles2-txt-target.hpp"
#include "../pipeline/gles2-pip-resource-set.hpp"

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

void gearoenix::gles2::command::Buffer::bind(const std::shared_ptr<render::texture::Target> &t)
{
	render::command::Buffer::bind(t);
}

void gearoenix::gles2::command::Buffer::play() const
{
	static_cast<const texture::Target *>(render_target.get())->bind();
	gl::uint bound_shader_program = static_cast<gl::uint>(-1);
	for (const std::shared_ptr<render::pipeline::ResourceSet> &prs : bound_resource_sets)
	{
		static_cast<const pipeline::ResourceSet *>(prs.get())->bind(bound_shader_program);
	}
	GXUNIMPLEMENTED;
}
