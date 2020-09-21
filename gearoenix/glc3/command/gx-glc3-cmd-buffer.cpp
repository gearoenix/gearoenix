#include "gx-glc3-cmd-buffer.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../render/pipeline/gx-rnd-pip-resource-set.hpp"
#include "../pipeline/gx-glc3-pip-resource-set.hpp"
#include "../texture/gx-glc3-txt-target.hpp"
#include "../texture/gx-glc3-txt-texture.hpp"

gearoenix::glc3::command::Buffer::Buffer(gearoenix::render::command::Type command_buffer_type) noexcept
    : render::command::Buffer(command_buffer_type)
{
}

std::pair<gearoenix::gl::uint, const gearoenix::glc3::texture::Target*> gearoenix::glc3::command::Buffer::play(
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
    for (const auto* const t : bound_texture_for_mipmap_generation)
        texture::Texture::generate_mipmap(t);
    return { bound_shader_program, target };
}

#endif