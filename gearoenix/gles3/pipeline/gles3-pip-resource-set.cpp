#include "gles3-pip-resource-set.hpp"
#ifdef GX_USE_OPENGL_ES3
#include "../../render/mesh/rnd-msh-mesh.hpp"
#include "../buffer/gles3-buf-index.hpp"
#include "../buffer/gles3-buf-vertex.hpp"
#include "../shader/gles3-shd-shader.hpp"

gearoenix::gles3::pipeline::ResourceSet::ResourceSet(const std::shared_ptr<shader::Shader>& shd)
    : shd(shd)
{
}

gearoenix::gles3::pipeline::ResourceSet::~ResourceSet() {}

void gearoenix::gles3::pipeline::ResourceSet::bind(gl::uint& bound_shader_program) const
{
    //if (shd->get_shader_program() != bound_shader_program) {
    bound_shader_program = shd->get_shader_program();
    shd->bind();
    //}
}
#endif