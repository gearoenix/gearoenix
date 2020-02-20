#include "gles2-cmd-buffer.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../render/pipeline/rnd-pip-resource-set.hpp"
#include "../pipeline/gles2-pip-resource-set.hpp"
#include "../texture/gles2-txt-target.hpp"

gearoenix::gl::uint gearoenix::gles2::command::Buffer::play(gl::uint bound_shader_program) const noexcept
{
    if (render_target != nullptr)
        static_cast<const texture::Target*>(render_target)->bind();
    for (const render::pipeline::ResourceSet* prs : bound_resource_sets)
        pipeline::ResourceSet::bind(prs, bound_shader_program);
    for (const render::command::Buffer* c : recorded_secondaries)
        bound_shader_program = static_cast<const Buffer*>(c)->play(bound_shader_program);
    return bound_shader_program;
}
#endif