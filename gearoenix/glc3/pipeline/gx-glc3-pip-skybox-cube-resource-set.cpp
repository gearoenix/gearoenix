#include "gx-glc3-pip-skybox-cube-resource-set.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../gl/gx-gl-loader.hpp"
#include "../../render/buffer/gx-rnd-buf-uniform.hpp"
#include "../../render/camera/gx-rnd-cmr-uniform.hpp"
#include "../../render/graph/node/gx-rnd-gr-nd-skybox-cube.hpp"
#include "../../render/material/gx-rnd-mat-skybox-cube.hpp"
#include "../../render/mesh/gx-rnd-msh-mesh.hpp"
#include "../buffer/gx-glc3-buf-index.hpp"
#include "../buffer/gx-glc3-buf-vertex.hpp"
#include "../shader/gx-glc3-shd-skybox-cube.hpp"
#include "../texture/gx-glc3-txt-cube.hpp"
#include "gx-glc3-pip-resource-set.hpp"
#include "gx-glc3-pip-skybox-cube.hpp"

gearoenix::glc3::pipeline::SkyboxCubeResourceSet::SkyboxCubeResourceSet(const std::shared_ptr<shader::SkyboxCube>& shd, std::shared_ptr<SkyboxCube const> pip) noexcept
    : render::pipeline::SkyboxCubeResourceSet(std::move(pip))
    , base(new glc3::pipeline::ResourceSet(shd))
{
}

gearoenix::glc3::pipeline::SkyboxCubeResourceSet::~SkyboxCubeResourceSet() noexcept = default;

void gearoenix::glc3::pipeline::SkyboxCubeResourceSet::bind_final(gl::uint& bound_shader_program) const noexcept
{
    GX_GLC3_PIP_RES_START_DRAWING_MESH
    GX_GLC3_PIP_RES_START_SHADER(SkyboxCube, shd)
    const auto* const material = material_uniform_buffer->get_ptr<render::material::SkyboxCube::Uniform>();
    GX_GLC3_PIP_RES_SET_UNIFORM(material_alpha, material->alpha)
    GX_GLC3_PIP_RES_SET_UNIFORM(material_alpha_cutoff, material->alpha_cutoff)
    GX_GLC3_PIP_RES_SET_TXT_CUBE(material_color, color)
    const auto* const camera = camera_uniform_buffer->get_ptr<render::camera::Uniform>();
    GX_GLC3_PIP_RES_SET_UNIFORM(camera_hdr_tune_mapping, camera->hdr_tune_mapping)
    GX_GLC3_PIP_RES_SET_UNIFORM(camera_gamma_correction, camera->gamma_correction)
    const auto* const node = node_uniform_buffer->get_ptr<render::graph::node::SkyboxCubeUniform>();
    GX_GLC3_PIP_RES_SET_UNIFORM(effect_mvp, node->mvp.data[0][0])
    GX_GLC3_PIP_RES_END_DRAWING_MESH
}
#endif