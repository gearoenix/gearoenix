#include "glc3-pip-resource-set.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../render/mesh/rnd-msh-mesh.hpp"
#include "../buffer/glc3-buf-index.hpp"
#include "../shader/glc3-shd-shader.hpp"
#include <utility>

gearoenix::glc3::pipeline::ResourceSet::ResourceSet(std::shared_ptr<shader::Shader> shd)
    : shd(std::move(shd))
{
}

gearoenix::glc3::pipeline::ResourceSet::~ResourceSet() = default;

void gearoenix::glc3::pipeline::ResourceSet::bind(gl::uint& bound_shader_program) const
{
    // TODO remove duplicated shader use
    //if (shd->get_shader_program() != bound_shader_program) {
    bound_shader_program = shd->get_shader_program();
    shd->bind();
    //}
}
#endif