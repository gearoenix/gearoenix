#include "gx-gles2-pip-skybox-equirectangular-resource-set.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../gl/gx-gl-loader.hpp"
#include "../../render/buffer/gx-rnd-buf-uniform.hpp"
#include "../../render/camera/gx-rnd-cmr-uniform.hpp"
#include "../../render/graph/node/gx-rnd-gr-nd-skybox-equirectangular.hpp"
#include "../../render/material/gx-rnd-mat-skybox-equirectangular.hpp"
#include "../../render/mesh/gx-rnd-msh-mesh.hpp"
#include "../buffer/gx-gles2-buf-index.hpp"
#include "../buffer/gx-gles2-buf-vertex.hpp"
#include "../shader/gx-gles2-shd-skybox-equirectangular.hpp"
#include "../texture/gx-gles2-txt-2d.hpp"
#include "gx-gles2-pip-resource-set.hpp"
#include "gx-gles2-pip-skybox-equirectangular.hpp"

gearoenix::gles2::pipeline::SkyboxEquirectangularResourceSet::SkyboxEquirectangularResourceSet(const std::shared_ptr<shader::SkyboxEquirectangular>& shd, std::shared_ptr<SkyboxEquirectangular const> pip) noexcept
    : render::pipeline::SkyboxEquirectangularResourceSet(std::move(pip))
    , base(new gles2::pipeline::ResourceSet(shd))
{
}

gearoenix::gles2::pipeline::SkyboxEquirectangularResourceSet::~SkyboxEquirectangularResourceSet() noexcept = default;

void gearoenix::gles2::pipeline::SkyboxEquirectangularResourceSet::bind_final(gl::uint& bound_shader_program) const noexcept
{
    GX_GLES2_PIP_RES_START_DRAWING_MESH
    GX_GLES2_PIP_RES_START_SHADER(SkyboxEquirectangular, shd)
    const auto* const material = material_uniform_buffer->get_ptr<render::material::SkyboxEquirectangular::Uniform>();
    GX_GLES2_PIP_RES_SET_UNIFORM(material_alpha, material->alpha)
    GX_GLES2_PIP_RES_SET_UNIFORM(material_alpha_cutoff, material->alpha_cutoff)
    GX_GLES2_PIP_RES_SET_TXT_2D(material_color, color)
    const auto* const node = node_uniform_buffer->get_ptr<render::graph::node::SkyboxEquirectangularUniform>();
    GX_GLES2_PIP_RES_SET_UNIFORM(effect_mvp, node->mvp.data[0][0])
    GX_GLES2_PIP_RES_END_DRAWING_MESH
}

#endif