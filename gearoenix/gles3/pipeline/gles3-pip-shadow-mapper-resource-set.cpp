#include "gles3-pip-shadow-mapper-resource-set.hpp"
#ifdef GX_USE_OPENGL_ES3
#include "../../gl/gl-loader.hpp"
#include "../../render/buffer/rnd-buf-uniform.hpp"
#include "../../render/camera/rnd-cmr-uniform.hpp"
#include "../../render/graph/node/rnd-gr-nd-shadow-mapper.hpp"
#include "../../render/light/rnd-lt-directional.hpp"
#include "../../render/material/rnd-mat-uniform.hpp"
#include "../../render/mesh/rnd-msh-mesh.hpp"
#include "../../render/model/rnd-mdl-uniform.hpp"
#include "../../render/scene/rnd-scn-uniform.hpp"
#include "../buffer/gles3-buf-index.hpp"
#include "../buffer/gles3-buf-vertex.hpp"
#include "../shader/gles3-shd-shadow-mapper.hpp"
#include "../texture/gles3-txt-2d.hpp"
#include "../texture/gles3-txt-cube.hpp"

gearoenix::gles3::pipeline::ShadowMapperResourceSet::ShadowMapperResourceSet(const std::shared_ptr<shader::ShadowMapper>& shd)
    : gles3::pipeline::ResourceSet(shd)
{
}

void gearoenix::gles3::pipeline::ShadowMapperResourceSet::bind(gl::uint& bound_shader_program) const
{
    GX_GLES3_PIPRES_START_DRAWING_MESH
    GX_GLES3_PIPRES_START_SHADER(ShadowMapper, shd);
    GX_GLES3_PIPRES_BIND_MATERIAL
    GX_GLES3_PIPRES_SET_UNIFORM(effect_mvp, *reinterpret_cast<const render::graph::node::ShadowMapperUniform*>(node_uniform_buffer->get_data())->mvp.data());
    GX_GLES3_PIPRES_END_DRAWING_MESH
}
#endif