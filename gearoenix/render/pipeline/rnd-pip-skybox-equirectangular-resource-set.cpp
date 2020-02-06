#include "rnd-pip-skybox-equirectangular-resource-set.hpp"
#include "../buffer/rnd-buf-framed-uniform.hpp"
#include "../material/rnd-mat-skybox-equirectangular.hpp"

gearoenix::render::pipeline::SkyboxEquirectangularResourceSet::~SkyboxEquirectangularResourceSet() noexcept = default;

void gearoenix::render::pipeline::SkyboxEquirectangularResourceSet::set_material(const material::Material* m) noexcept
{

    material_uniform_buffer = m->get_uniform_buffers()->get_buffer();
    auto* const sky_equirectangular_mat = reinterpret_cast<const material::SkyboxEquirectangular*>(m);
    color = sky_equirectangular_mat->get_color_texture().get();
}

void gearoenix::render::pipeline::SkyboxEquirectangularResourceSet::set_mesh(const mesh::Mesh* m) noexcept
{
    msh = m;
}

void gearoenix::render::pipeline::SkyboxEquirectangularResourceSet::set_node_uniform_buffer(buffer::Uniform* b) noexcept
{
    node_uniform_buffer = b;
}

void gearoenix::render::pipeline::SkyboxEquirectangularResourceSet::clean() noexcept
{
    material_uniform_buffer = nullptr;
    node_uniform_buffer = nullptr;
    msh = nullptr;
    color = nullptr;
}
