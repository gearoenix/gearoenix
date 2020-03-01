#include "glc3-pip-irradiance-convoluter-resource-set.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../gl/gl-loader.hpp"
#include "../../render/buffer/rnd-buf-uniform.hpp"
#include "../../render/camera/rnd-cmr-uniform.hpp"
#include "../../render/graph/node/rnd-gr-nd-irradiance-convoluter.hpp"
#include "../../render/mesh/rnd-msh-mesh.hpp"
#include "../buffer/glc3-buf-index.hpp"
#include "../buffer/glc3-buf-vertex.hpp"
#include "../shader/glc3-shd-irradiance-convoluter.hpp"
#include "../texture/glc3-txt-cube.hpp"
#include "glc3-pip-irradiance-convoluter.hpp"
#include "glc3-pip-resource-set.hpp"

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
    GX_GLC3_PIP_RES_END_DRAWING_MESH
#ifdef GX_DEBUG_GL_CLASS_3
    gl::Loader::check_for_error();
#endif
}

#endif