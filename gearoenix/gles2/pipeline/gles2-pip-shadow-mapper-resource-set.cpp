#include "gles2-pip-shadow-mapper-resource-set.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../render/buffer/rnd-buf-uniform.hpp"
#include "../../render/camera/rnd-cmr-uniform.hpp"
#include "../../render/graph/node/rnd-gr-nd-shadow-mapper.hpp"
#include "../../render/light/rnd-lt-directional.hpp"
#include "../../render/material/rnd-mat-uniform.hpp"
#include "../../render/mesh/rnd-msh-mesh.hpp"
#include "../../render/model/rnd-mdl-uniform.hpp"
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
#ifdef GX_DEBUG_GLES2
    gl::Loader::check_for_error();
#endif
    GX_GLES2_PIPRES_START_DRAWING_MESH;
#ifdef GX_DEBUG_GLES2
    gl::Loader::check_for_error();
#endif
    GX_GLES2_PIPRES_START_SHADER(ShadowMapper, shd);
#ifdef GX_DEBUG_GLES2
    gl::Loader::check_for_error();
#endif
    GX_GLES2_PIPRES_BIND_MATERIAL
#ifdef GX_DEBUG_GLES2
    gl::Loader::check_for_error();
#endif
    GX_GLES2_PIPRES_SET_UNIFORM(effect_mvp, *reinterpret_cast<const render::graph::node::ShadowMapperUniform*>(node_uniform_buffer->get_data())->mvp.data());
#ifdef GX_DEBUG_GLES2
    gl::Loader::check_for_error();
#endif
    GX_GLES2_PIPRES_END_DRAWING_MESH;
#ifdef GX_DEBUG_GLES2
    gl::Loader::check_for_error();
#endif
}
