#include "glc3-pip-shadow-mapper-resource-set.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../gl/gl-loader.hpp"
#include "../../render/buffer/rnd-buf-uniform.hpp"
#include "../../render/camera/rnd-cmr-uniform.hpp"
#include "../../render/graph/node/rnd-gr-nd-shadow-mapper.hpp"
#include "../../render/light/rnd-lt-directional.hpp"
#include "../../render/mesh/rnd-msh-mesh.hpp"
#include "../buffer/glc3-buf-index.hpp"
#include "../buffer/glc3-buf-vertex.hpp"
#include "../shader/glc3-shd-shadow-mapper.hpp"
#include "../texture/glc3-txt-2d.hpp"

gearoenix::glc3::pipeline::ShadowMapperResourceSet::ShadowMapperResourceSet(const std::shared_ptr<shader::ShadowMapper>& shd) noexcept
    : glc3::pipeline::ResourceSet(shd)
{
}

void gearoenix::glc3::pipeline::ShadowMapperResourceSet::bind(gl::uint& bound_shader_program) const noexcept
{
    GX_GLC3_PIP_RES_START_DRAWING_MESH
    GX_GLC3_PIP_RES_START_SHADER(ShadowMapper, shd)
    GX_GLC3_PIP_RES_SET_TXT_2D(material_color, color)
    const auto* const node = node_uniform_buffer->get_ptr<render::graph::node::ShadowMapperUniform>();
    GX_GLC3_PIP_RES_SET_UNIFORM(effect_mvp, *(node->mvp.data()))
    GX_GLC3_PIP_RES_SET_UNIFORM(effect_mvp, node->alpha)
    GX_GLC3_PIP_RES_SET_UNIFORM(effect_mvp, node->alpha_cutoff)
    GX_GLC3_PIP_RES_END_DRAWING_MESH
}
#endif