#include "gles2-pip-unlit-resource-set.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../gl/gl-loader.hpp"
#include "../../render/buffer/rnd-buf-uniform.hpp"
#include "../../render/camera/rnd-cmr-uniform.hpp"
#include "../../render/graph/node/rnd-gr-nd-unlit.hpp"
#include "../../render/light/rnd-lt-directional.hpp"
#include "../../render/material/rnd-mat-unlit.hpp"
#include "../../render/mesh/rnd-msh-mesh.hpp"
#include "../buffer/gles2-buf-index.hpp"
#include "../buffer/gles2-buf-vertex.hpp"
#include "../shader/gles2-shd-unlit.hpp"
#include "../texture/gles2-txt-2d.hpp"

gearoenix::gles2::pipeline::UnlitResourceSet::UnlitResourceSet(const std::shared_ptr<shader::Unlit>& shd) noexcept
    : gles2::pipeline::ResourceSet(shd)
{
}

void gearoenix::gles2::pipeline::UnlitResourceSet::bind(gl::uint& bound_shader_program) const noexcept
{
    GX_GLES2_PIP_RES_START_DRAWING_MESH
    GX_GLES2_PIP_RES_START_SHADER(Unlit, shd)
    const auto* const material = material_uniform_buffer->get_ptr<render::material::Unlit::Uniform>();
    GX_GLES2_PIP_RES_SET_UNIFORM(material_alpha, material->alpha)
    GX_GLES2_PIP_RES_SET_UNIFORM(material_alpha_cutoff, material->alpha_cutoff)
    GX_GLES2_PIP_RES_SET_TXT_2D(material_color, color)
    const auto* const node = node_uniform_buffer->get_ptr<render::graph::node::UnlitUniform>();
    GX_GLES2_PIP_RES_SET_UNIFORM(effect_mvp, *(node->mvp.data()))
    GX_GLES2_PIP_RES_END_DRAWING_MESH
}

#endif