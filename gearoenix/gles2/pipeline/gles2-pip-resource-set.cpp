#include "gles2-pip-resource-set.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../render/mesh/rnd-msh-mesh.hpp"
#include "../../render/pipeline/rnd-pip-pipeline.hpp"
#include "../buffer/gles2-buf-index.hpp"
#include "../shader/gles2-shd-shader.hpp"
#include "gles2-pip-forward-pbr-resource-set.hpp"
#include "gles2-pip-irradiance-convoluter-resource-set.hpp"
#include "gles2-pip-shadow-mapper-resource-set.hpp"
#include "gles2-pip-skybox-cube-resource-set.hpp"
#include "gles2-pip-skybox-equirectangular-resource-set.hpp"
#include "gles2-pip-unlit-resource-set.hpp"
#include <utility>

gearoenix::gles2::pipeline::ResourceSet::ResourceSet(std::shared_ptr<shader::Shader> shd) noexcept
    : shd(std::move(shd))
{
}

gearoenix::gles2::pipeline::ResourceSet::~ResourceSet() noexcept = default;

void gearoenix::gles2::pipeline::ResourceSet::bind(gl::uint& bound_shader_program) const noexcept
{
    // TODO
    //if (shd->get_shader_program() != bound_shader_program) {
    bound_shader_program = shd->get_shader_program();
    shd->bind();
    //}
}

void gearoenix::gles2::pipeline::ResourceSet::bind(
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
    default:
        GX_UNIMPLEMENTED
    }
}

#endif