#include "glc3-pip-unlit-resource-set.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../gl/gl-loader.hpp"
#include "../../render/buffer/rnd-buf-uniform.hpp"
#include "../../render/camera/rnd-cmr-uniform.hpp"
#include "../../render/graph/node/rnd-gr-nd-unlit.hpp"
#include "../../render/light/rnd-lt-directional.hpp"
#include "../../render/material/rnd-mat-unlit.hpp"
#include "../../render/mesh/rnd-msh-mesh.hpp"
#include "../buffer/glc3-buf-index.hpp"
#include "../buffer/glc3-buf-vertex.hpp"
#include "../shader/glc3-shd-unlit.hpp"
#include "../texture/glc3-txt-2d.hpp"
#include "glc3-pip-resource-set.hpp"
#include "glc3-pip-unlit.hpp"

gearoenix::glc3::pipeline::UnlitResourceSet::UnlitResourceSet(const std::shared_ptr<shader::Unlit>& shd, std::shared_ptr<Unlit const> pip) noexcept
    : render::pipeline::UnlitResourceSet(std::move(pip))
    , base(new glc3::pipeline::ResourceSet(shd))
{
}

gearoenix::glc3::pipeline::UnlitResourceSet::~UnlitResourceSet() noexcept = default;

void gearoenix::glc3::pipeline::UnlitResourceSet::bind_final(gl::uint& bound_shader_program) const noexcept
{
    GX_GLC3_PIP_RES_START_DRAWING_MESH
    GX_GLC3_PIP_RES_START_SHADER(Unlit, shd)
    const auto* const material = material_uniform_buffer->get_ptr<render::material::Unlit::Uniform>();
    GX_GLC3_PIP_RES_SET_UNIFORM(material_alpha, material->alpha)
    GX_GLC3_PIP_RES_SET_UNIFORM(material_alpha_cutoff, material->alpha_cutoff)
    GX_GLC3_PIP_RES_SET_TXT_2D(material_color, color)
    const auto* const node = node_uniform_buffer->get_ptr<render::graph::node::UnlitUniform>();
    GX_GLC3_PIP_RES_SET_UNIFORM(effect_mvp, *(node->mvp.data()))
    GX_GLC3_PIP_RES_END_DRAWING_MESH
}

#endif