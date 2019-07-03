#include "gles3-cmd-buffer.hpp"
#ifdef GX_USE_OPENGL_ES3
#include "../../render/pipeline/rnd-pip-resource-set.hpp"
#include "../../system/sys-log.hpp"
#include "../pipeline/gles3-pip-resource-set.hpp"
#include "../texture/gles3-txt-target.hpp"

gearoenix::gl::uint gearoenix::gles3::command::Buffer::play(gl::uint bound_shader_program) const noexcept
{
    if (render_target != nullptr)
        reinterpret_cast<const texture::Target*>(render_target)->bind();
    for (const render::pipeline::ResourceSet* prs : bound_resource_sets) {
        dynamic_cast<const pipeline::ResourceSet*>(prs)->bind(bound_shader_program);
    }
    for (const render::command::Buffer* c : recorded_secondaries) {
        bound_shader_program = reinterpret_cast<const Buffer*>(c)->play(bound_shader_program);
    }
    return bound_shader_program;
}
#endif