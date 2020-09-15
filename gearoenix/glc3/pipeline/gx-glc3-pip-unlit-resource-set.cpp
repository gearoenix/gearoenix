#include "gx-glc3-pip-unlit-resource-set.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../gl/gx-gl-loader.hpp"
#include "../../render/buffer/gx-rnd-buf-uniform.hpp"
#include "../../render/camera/gx-rnd-cmr-uniform.hpp"
#include "../../render/graph/node/gx-rnd-gr-nd-unlit.hpp"
#include "../../render/material/gx-rnd-mat-unlit.hpp"
#include "../../render/mesh/gx-rnd-msh-mesh.hpp"
#include "../buffer/gx-glc3-buf-index.hpp"
#include "../buffer/gx-glc3-buf-vertex.hpp"
#include "../shader/gx-glc3-shd-unlit.hpp"
#include "../texture/gx-glc3-txt-2d.hpp"
#include "gx-glc3-pip-resource-set.hpp"
#include "gx-glc3-pip-unlit.hpp"

gearoenix::glc3::pipeline::UnlitResourceSet::UnlitResourceSet(const std::shared_ptr<shader::Unlit>& shd, std::shared_ptr<Unlit const> pip) noexcept
    : render::pipeline::UnlitResourceSet(std::move(pip))
    , base(new glc3::pipeline::ResourceSet(shd))
{
}

gearoenix::glc3::pipeline::UnlitResourceSet::~UnlitResourceSet() noexcept = default;

void gearoenix::glc3::pipeline::UnlitResourceSet::bind_final(gl::uint& bound_shader_program) const noexcept
{
#ifdef GX_DEBUG_GL_CLASS_3
    gl::Loader::check_for_error();
#endif
    GX_GLC3_PIP_RES_START_DRAWING_MESH
    GX_GLC3_PIP_RES_START_SHADER(Unlit, shd)
#ifdef GX_DEBUG_GL_CLASS_3
    gl::Loader::check_for_error();
#endif
    const auto* const material = material_uniform_buffer->get_ptr<render::material::Unlit::Uniform>();
    GX_GLC3_PIP_RES_SET_UNIFORM(material_alpha, material->alpha)
    GX_GLC3_PIP_RES_SET_UNIFORM(material_alpha_cutoff, material->alpha_cutoff)
#ifdef GX_DEBUG_GL_CLASS_3
    gl::Loader::check_for_error();
#endif
    GX_GLC3_PIP_RES_SET_TXT_2D(material_color, color)
#ifdef GX_DEBUG_GL_CLASS_3
    gl::Loader::check_for_error();
#endif
    const auto* const node = node_uniform_buffer->get_ptr<render::graph::node::UnlitUniform>();
    GX_GLC3_PIP_RES_SET_UNIFORM(effect_mvp, node->mvp.data[0][0])
    GX_GLC3_PIP_RES_END_DRAWING_MESH
#ifdef GX_DEBUG_GL_CLASS_3
    gl::Loader::check_for_error();
#endif
}

#endif