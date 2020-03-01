#include "glc3-cmd-buffer.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../render/pipeline/rnd-pip-resource-set.hpp"
#include "../pipeline/glc3-pip-resource-set.hpp"
#include "../texture/glc3-txt-target.hpp"

std::pair<gearoenix::gl::uint, const gearoenix::glc3::texture::Target*> gearoenix::glc3::command::Buffer::play(
    const texture::Target* const bound_target,
    gl::uint bound_shader_program) const noexcept
{
    const auto* const target = static_cast<const texture::Target*>(render_target);
    if (render_target != nullptr && bound_target != render_target)
        target->bind(bound_target);
    for (const render::pipeline::ResourceSet* prs : bound_resource_sets)
        pipeline::ResourceSet::bind(prs, bound_shader_program);
    for (const render::command::Buffer* c : recorded_secondaries)
        bound_shader_program = static_cast<const Buffer*>(c)->play(target, bound_shader_program).first;
    return { bound_shader_program, target };
}
#endif