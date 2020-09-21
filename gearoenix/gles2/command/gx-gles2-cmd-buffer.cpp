#include "gx-gles2-cmd-buffer.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../render/pipeline/gx-rnd-pip-resource-set.hpp"
#include "../pipeline/gx-gles2-pip-resource-set.hpp"
#include "../texture/gx-gles2-txt-target.hpp"

gearoenix::gles2::command::Buffer::Buffer(
    const render::command::Type command_buffer_type) noexcept
    : render::command::Buffer(command_buffer_type)
{
}

std::pair<gearoenix::gl::uint, const gearoenix::gles2::texture::Target*> gearoenix::gles2::command::Buffer::play(
    const texture::Target* const bound_target,
    gl::uint bound_shader_program) const noexcept
{
    const auto* const target = dynamic_cast<const texture::Target*>(render_target);
    if (render_target != nullptr && bound_target != render_target)
        target->bind(bound_target);
    for (const render::pipeline::ResourceSet* prs : bound_resource_sets)
        pipeline::ResourceSet::bind(prs, bound_shader_program);
    for (const render::command::Buffer* c : recorded_secondaries)
        bound_shader_program = dynamic_cast<const Buffer*>(c)->play(target, bound_shader_program).first;
    return { bound_shader_program, target };
}

#endif