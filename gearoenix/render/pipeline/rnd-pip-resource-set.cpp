#include "rnd-pip-resource-set.hpp"
#include "../buffer/rnd-buf-framed-uniform.hpp"
#include "../camera/rnd-cmr-camera.hpp"
#include "../graph/node/rnd-gr-nd-forward-pbr-directional-shadow.hpp"
#include "../light/rnd-lt-light.hpp"
#include "../material/rnd-mat-material.hpp"
#include "../mesh/rnd-msh-mesh.hpp"
#include "../model/rnd-mdl-model.hpp"
#include "../scene/rnd-scn-scene.hpp"

#define GX_HELPER(c, cc)                                                                  \
    void gearoenix::render::pipeline::ResourceSet::set_##c(const c::cc* const o) noexcept \
    {                                                                                     \
        c##_uniform_buffer = o->get_uniform_buffers()->get_buffer();                      \
    }

GX_HELPER(scene, Scene)
GX_HELPER(camera, Camera)

void gearoenix::render::pipeline::ResourceSet::set_light(const light::Light* const l) noexcept
{
    const std::shared_ptr<buffer::FramedUniform>& fub = l->get_uniform_buffers();
    if (fub == nullptr)
        light_uniform_buffer = nullptr;
    else
        light_uniform_buffer = fub->get_buffer();
}

GX_HELPER(model, Model)

void gearoenix::render::pipeline::ResourceSet::set_mesh(const mesh::Mesh* const m) noexcept
{
    msh = m;
}

void gearoenix::render::pipeline::ResourceSet::set_material(const material::Material* m) noexcept
{
    material_uniform_buffer = m->get_uniform_buffers()->get_buffer();
    color = m->get_color();
    metallic_roughness = m->get_metallic_roughness();
    normal = m->get_normal();
    emissive = m->get_emissive();
}

#undef GX_HELPER

void gearoenix::render::pipeline::ResourceSet::set_node_uniform_buffer(buffer::Uniform* b)
{
    node_uniform_buffer = b;
}

void gearoenix::render::pipeline::ResourceSet::clean()
{
    scene_uniform_buffer = nullptr;
    camera_uniform_buffer = nullptr;
    light_uniform_buffer = nullptr;
    model_uniform_buffer = nullptr;
    material_uniform_buffer = nullptr;
    node_uniform_buffer = nullptr;
    msh = nullptr;
    color = nullptr;
    metallic_roughness = nullptr;
    normal = nullptr;
    emissive = nullptr;
}
