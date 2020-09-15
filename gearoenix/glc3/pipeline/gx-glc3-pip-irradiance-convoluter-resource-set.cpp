#include "gx-glc3-pip-irradiance-convoluter-resource-set.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../gl/gx-gl-loader.hpp"
#include "../../render/buffer/gx-rnd-buf-uniform.hpp"
#include "../../render/camera/gx-rnd-cmr-uniform.hpp"
#include "../../render/graph/node/gx-rnd-gr-nd-irradiance-convoluter.hpp"
#include "../../render/mesh/gx-rnd-msh-mesh.hpp"
#include "../buffer/gx-glc3-buf-index.hpp"
#include "../buffer/gx-glc3-buf-uniform.hpp"
#include "../buffer/gx-glc3-buf-vertex.hpp"
#include "../shader/gx-glc3-shd-irradiance-convoluter.hpp"
#include "../texture/gx-glc3-txt-cube.hpp"
#include "gx-glc3-pip-irradiance-convoluter.hpp"
#include "gx-glc3-pip-resource-set.hpp"

gearoenix::glc3::pipeline::IrradianceConvoluterResourceSet::IrradianceConvoluterResourceSet(const std::shared_ptr<shader::IrradianceConvoluter>& shd, std::shared_ptr<IrradianceConvoluter const> pip) noexcept
    : render::pipeline::IrradianceConvoluterResourceSet(std::move(pip))
    , base(new glc3::pipeline::ResourceSet(shd))
{
}

gearoenix::glc3::pipeline::IrradianceConvoluterResourceSet::~IrradianceConvoluterResourceSet() noexcept = default;

void gearoenix::glc3::pipeline::IrradianceConvoluterResourceSet::bind_final(gl::uint& bound_shader_program) const noexcept
{
#ifdef GX_DEBUG_GL_CLASS_3
    gl::Loader::check_for_error();
#endif
    GX_GLC3_PIP_RES_START_DRAWING_MESH
    GX_GLC3_PIP_RES_START_SHADER(IrradianceConvoluter, shd)
    GX_GLC3_PIP_RES_SET_TXT_CUBE(environment, environment)
    const auto* const node = node_uniform_buffer->get_ptr<render::graph::node::IrradianceConvoluterUniform>();
    GX_GLC3_PIP_RES_SET_UNIFORM(mvp, node->mvp.data[0][0])
    GX_GLC3_PIP_RES_END_DRAWING_MESH
#ifdef GX_DEBUG_GL_CLASS_3
    gl::Loader::check_for_error();
#endif
}

#endif