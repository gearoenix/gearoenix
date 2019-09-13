#include "rnd-pip-resource-set.hpp"
#include "../buffer/rnd-buf-framed-uniform.hpp"
#include "../camera/rnd-cmr-camera.hpp"
#include "../graph/node/rnd-gr-nd-forward-pbr.hpp"
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
GX_HELPER(model, Model)

void gearoenix::render::pipeline::ResourceSet::set_mesh(const mesh::Mesh* const m) noexcept
{
    msh = m;
}

void gearoenix::render::pipeline::ResourceSet::set_material(const material::Material* m) noexcept
{
    material_uniform_buffer = m->get_uniform_buffers()->get_buffer();
    color = m->get_color_texture().get();
    metallic_roughness = m->get_metallic_roughness().get();
    normal = m->get_normal().get();
    emissive = m->get_emissive().get();
}

#undef GX_HELPER

void gearoenix::render::pipeline::ResourceSet::set_node_uniform_buffer(buffer::Uniform* b) noexcept
{
    node_uniform_buffer = b;
}

void gearoenix::render::pipeline::ResourceSet::clean() noexcept
{
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
