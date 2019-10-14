#include "gles2-pip-shadow-mapper-resource-set.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../gl/gl-loader.hpp"
#include "../../render/buffer/rnd-buf-uniform.hpp"
#include "../../render/camera/rnd-cmr-uniform.hpp"
#include "../../render/graph/node/rnd-gr-nd-shadow-mapper.hpp"
#include "../../render/light/rnd-lt-directional.hpp"
#include "../../render/material/rnd-mat-uniform.hpp"
#include "../../render/mesh/rnd-msh-mesh.hpp"
#include "../../render/scene/rnd-scn-uniform.hpp"
#include "../buffer/gles2-buf-index.hpp"
#include "../buffer/gles2-buf-vertex.hpp"
#include "../shader/gles2-shd-shadow-mapper.hpp"
#include "../texture/gles2-txt-2d.hpp"
#include "../texture/gles2-txt-cube.hpp"

gearoenix::gles2::pipeline::ShadowMapperResourceSet::ShadowMapperResourceSet(const std::shared_ptr<shader::ShadowMapper>& shd)
    : gles2::pipeline::ResourceSet(shd)
{
}

void gearoenix::gles2::pipeline::ShadowMapperResourceSet::bind(gl::uint& bound_shader_program) const
{
    GX_GLES2_PIPRES_START_DRAWING_MESH
    GX_GLES2_PIPRES_START_SHADER(ShadowMapper, shd);
    GX_GLES2_PIPRES_BIND_MATERIAL
    GX_GLES2_PIPRES_SET_UNIFORM(effect_mvp, *(node_uniform_buffer->get_ptr<render::graph::node::ShadowMapperUniform>()->mvp.data()))
    GX_GLES2_PIPRES_END_DRAWING_MESH;
}
#endif