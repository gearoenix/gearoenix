#include "rnd-pip-sky-equirectangular-resource-set.hpp"
#include "../buffer/rnd-buf-framed-uniform.hpp"
#include "../material/rnd-mat-sky-equirectangular.hpp"

void gearoenix::render::pipeline::SkyEquirectangularResourceSet::set_material(const material::Material* m) noexcept
{

    material_uniform_buffer = m->get_uniform_buffers()->get_buffer();
    auto* const sky_equirectangular_mat = reinterpret_cast<const material::SkyEquirectangular*>(m);
    color = sky_equirectangular_mat->get_color_texture().get();
}

void gearoenix::render::pipeline::SkyEquirectangularResourceSet::set_mesh(const mesh::Mesh* m) noexcept
{
    msh = m;
}

void gearoenix::render::pipeline::SkyEquirectangularResourceSet::set_node_uniform_buffer(buffer::Uniform* b) noexcept
{
    node_uniform_buffer = b;
}

void gearoenix::render::pipeline::SkyEquirectangularResourceSet::clean() noexcept
{
    material_uniform_buffer = nullptr;
    node_uniform_buffer = nullptr;
    msh = nullptr;
    color = nullptr;
}