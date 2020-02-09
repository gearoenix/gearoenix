#include "gles2-pip-resource-set.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../render/mesh/rnd-msh-mesh.hpp"
#include "../buffer/gles2-buf-index.hpp"
#include "../shader/gles2-shd-shader.hpp"
#include <utility>

gearoenix::gles2::pipeline::ResourceSet::ResourceSet(std::shared_ptr<shader::Shader> shd) noexcept
    : shd(std::move(shd))
{
}

gearoenix::gles2::pipeline::ResourceSet::~ResourceSet() noexcept = default;

void gearoenix::gles2::pipeline::ResourceSet::bind(gl::uint& bound_shader_program) const noexcept
{
    // TODO
    //if (shd->get_shader_program() != bound_shader_program) {
    bound_shader_program = shd->get_shader_program();
    shd->bind();
    //}
}

void gearoenix::gles2::pipeline::ResourceSet::bind(
    const render::pipeline::ResourceSet* const prs,
    uint& bound_shader_program) noexcept
{
    GXUNIMPLEMENTED
}

#endif