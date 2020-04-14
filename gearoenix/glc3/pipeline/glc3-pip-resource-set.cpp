#include "glc3-pip-resource-set.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../render/mesh/rnd-msh-mesh.hpp"
#include "../../render/pipeline/rnd-pip-pipeline.hpp"
#include "../buffer/glc3-buf-index.hpp"
#include "../shader/glc3-shd-shader.hpp"
#include "glc3-pip-forward-pbr-resource-set.hpp"
#include "glc3-pip-irradiance-convoluter-resource-set.hpp"
#include "glc3-pip-radiance-convoluter-resource-set.hpp"
#include "glc3-pip-shadow-mapper-resource-set.hpp"
#include "glc3-pip-skybox-cube-resource-set.hpp"
#include "glc3-pip-skybox-equirectangular-resource-set.hpp"
#include "glc3-pip-unlit-resource-set.hpp"
#include <utility>

gearoenix::glc3::pipeline::ResourceSet::ResourceSet(std::shared_ptr<shader::Shader> shd) noexcept
    : shd(std::move(shd))
{
}

gearoenix::glc3::pipeline::ResourceSet::~ResourceSet() noexcept = default;

void gearoenix::glc3::pipeline::ResourceSet::bind(gl::uint& bound_shader_program) const noexcept
{
    // TODO remove duplicated shader use
    //if (shd->get_shader_program() != bound_shader_program) {
    bound_shader_program = shd->get_shader_program();
    shd->bind();
    //}
}

void gearoenix::glc3::pipeline::ResourceSet::bind(
    const render::pipeline::ResourceSet* const prs,
    gl::uint& bound_shader_program) noexcept
{
    switch (prs->get_pip()->get_pipeline_type()) {
    case render::pipeline::Type::ShadowMapper:
        static_cast<const ShadowMapperResourceSet*>(prs)->bind_final(bound_shader_program);
        break;
    case render::pipeline::Type::Unlit:
        static_cast<const UnlitResourceSet*>(prs)->bind_final(bound_shader_program);
        break;
    case render::pipeline::Type::SkyboxCube:
        static_cast<const SkyboxCubeResourceSet*>(prs)->bind_final(bound_shader_program);
        break;
    case render::pipeline::Type::SkyboxEquirectangular:
        static_cast<const SkyboxEquirectangularResourceSet*>(prs)->bind_final(bound_shader_program);
        break;
    case render::pipeline::Type::ForwardPbr:
        static_cast<const ForwardPbrResourceSet*>(prs)->bind_final(bound_shader_program);
        break;
    case render::pipeline::Type::IrradianceConvoluter:
        static_cast<const IrradianceConvoluterResourceSet*>(prs)->bind_final(bound_shader_program);
        break;
    case render::pipeline::Type::RadianceConvoluter:
        static_cast<const RadianceConvoluterResourceSet*>(prs)->bind_final(bound_shader_program);
        break;
    default:
        GXUNIMPLEMENTED
    }
}

#endif