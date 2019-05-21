#include "gles2-cmd-buffer.hpp"
#include "../../render/pipeline/rnd-pip-resource-set.hpp"
#include "../../system/sys-log.hpp"
#include "../pipeline/gles2-pip-resource-set.hpp"
#include "../texture/gles2-txt-target.hpp"

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

void gearoenix::gles2::command::Buffer::bind(const std::shared_ptr<render::pipeline::ResourceSet>& r)
{
    render::command::Buffer::bind(r);
}

void gearoenix::gles2::command::Buffer::bind(render::texture::Target* const t)
{
    render::command::Buffer::bind(t);
}

gearoenix::gl::uint gearoenix::gles2::command::Buffer::play(gl::uint bound_shader_program) const
{
    if (render_target != nullptr)
        static_cast<const texture::Target*>(render_target)->bind();
    for (const std::shared_ptr<render::pipeline::ResourceSet>& prs : bound_resource_sets) {
        dynamic_cast<const pipeline::ResourceSet*>(prs.get())->bind(bound_shader_program);
    }
    for (const std::shared_ptr<render::command::Buffer>& c : recored_secondaries) {
        bound_shader_program = static_cast<const Buffer*>(c.get())->play(bound_shader_program);
    }
    return bound_shader_program;
}
