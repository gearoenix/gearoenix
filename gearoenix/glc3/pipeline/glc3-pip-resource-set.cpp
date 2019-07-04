#include "glc3-pip-resource-set.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../render/mesh/rnd-msh-mesh.hpp"
#include "../buffer/glc3-buf-index.hpp"
#include "../buffer/glc3-buf-vertex.hpp"
#include "../shader/glc3-shd-shader.hpp"

gearoenix::glc3::pipeline::ResourceSet::ResourceSet(const std::shared_ptr<shader::Shader>& shd)
    : shd(shd)
{
}

gearoenix::glc3::pipeline::ResourceSet::~ResourceSet() {}

void gearoenix::glc3::pipeline::ResourceSet::bind(gl::uint& bound_shader_program) const
{
    //if (shd->get_shader_program() != bound_shader_program) {
    bound_shader_program = shd->get_shader_program();
    shd->bind();
    //}
}
#endif