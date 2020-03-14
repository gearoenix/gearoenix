#include "rnd-pip-forward-pbr-resource-set.hpp"
#include "../../core/cr-static.hpp"
#include "../buffer/rnd-buf-framed-uniform.hpp"
#include "../camera/rnd-cmr-camera.hpp"
#include "../light/rnd-lt-light.hpp"
#include "../material/rnd-mat-material.hpp"
#include "../material/rnd-mat-pbr.hpp"
#include "../mesh/rnd-msh-mesh.hpp"
#include "../model/rnd-mdl-model.hpp"
#include "../scene/rnd-scn-scene.hpp"
#include "rnd-pip-forward-pbr.hpp"

gearoenix::render::pipeline::ForwardPbrResourceSet::ForwardPbrResourceSet(std::shared_ptr<ForwardPbr const> pip) noexcept
    : ResourceSet(std::move(pip))
{
}

gearoenix::render::pipeline::ForwardPbrResourceSet::~ForwardPbrResourceSet() noexcept = default;

#define GX_HELPER(c, cc)                                                                            \
    void gearoenix::render::pipeline::ForwardPbrResourceSet::set_##c(const c::cc* const o) noexcept \
    {                                                                                               \
        c##_uniform_buffer = o->get_uniform_buffers()->get_buffer();                                \
    }

GX_HELPER(scene, Scene)
GX_HELPER(camera, Camera)
GX_HELPER(model, Model)

#undef GX_HELPER

void gearoenix::render::pipeline::ForwardPbrResourceSet::set_mesh(const mesh::Mesh* const m) noexcept
{
    msh = m;
}

void gearoenix::render::pipeline::ForwardPbrResourceSet::set_material(const material::Material* m) noexcept
{
    material_uniform_buffer = m->get_uniform_buffers()->get_buffer();
    auto* const pbr_mat = reinterpret_cast<const material::Pbr*>(m);
    color = pbr_mat->get_color_texture().get();
    metallic_roughness = pbr_mat->get_metallic_roughness_texture().get();
    normal = pbr_mat->get_normal_texture().get();
    emissive = pbr_mat->get_emission_texture().get();
    irradiance = pbr_mat->get_irradiance();
    radiance = pbr_mat->get_radiance();
}

void gearoenix::render::pipeline::ForwardPbrResourceSet::set_node_uniform_buffer(buffer::Uniform* b) noexcept
{
    node_uniform_buffer = b;
}

void gearoenix::render::pipeline::ForwardPbrResourceSet::set_ambient_occlusion(texture::Texture2D* const t) noexcept
{
    ambient_occlusion = t;
}

void gearoenix::render::pipeline::ForwardPbrResourceSet::set_directional_lights_shadow_map(texture::Texture2D* const t, const int light_index, const int cas_index) noexcept
{
    directional_lights_shadow_maps[light_index][cas_index] = t;
}

void gearoenix::render::pipeline::ForwardPbrResourceSet::set_brdflut(texture::Texture2D* const t) noexcept
{
    brdflut = t;
}

void gearoenix::render::pipeline::ForwardPbrResourceSet::clean() noexcept
{
    irradiance = nullptr;
    radiance = nullptr;
    ambient_occlusion = nullptr;
    GX_SET_ARRAY_ZERO(directional_lights_shadow_maps)
    brdflut = nullptr;
    scene_uniform_buffer = nullptr;
    camera_uniform_buffer = nullptr;
    model_uniform_buffer = nullptr;
    material_uniform_buffer = nullptr;
    node_uniform_buffer = nullptr;
    msh = nullptr;
    color = nullptr;
    metallic_roughness = nullptr;
    normal = nullptr;
    emissive = nullptr;
}
