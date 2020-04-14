#include "rnd-pip-skybox-cube-resource-set.hpp"
#include "../buffer/rnd-buf-framed-uniform.hpp"
#include "../camera/rnd-cmr-camera.hpp"
#include "../material/rnd-mat-skybox-cube.hpp"
#include "rnd-pip-skybox-cube.hpp"

gearoenix::render::pipeline::SkyboxCubeResourceSet::SkyboxCubeResourceSet(
    std::shared_ptr<SkyboxCube const> pip) noexcept
    : ResourceSet(std::move(pip))
{
}

gearoenix::render::pipeline::SkyboxCubeResourceSet::~SkyboxCubeResourceSet() noexcept = default;

void gearoenix::render::pipeline::SkyboxCubeResourceSet::set_material(const material::SkyboxCube* const m) noexcept
{
    material_uniform_buffer = m->get_uniform_buffers()->get_buffer();
    color = m->get_color_texture().get();
}

void gearoenix::render::pipeline::SkyboxCubeResourceSet::set_mesh(const mesh::Mesh* m) noexcept
{
    msh = m;
}

void gearoenix::render::pipeline::SkyboxCubeResourceSet::set_node_uniform_buffer(buffer::Uniform* b) noexcept
{
    node_uniform_buffer = b;
}

void gearoenix::render::pipeline::SkyboxCubeResourceSet::clean() noexcept
{
    material_uniform_buffer = nullptr;
    node_uniform_buffer = nullptr;
    msh = nullptr;
    color = nullptr;
}

void gearoenix::render::pipeline::SkyboxCubeResourceSet::set_camera(const camera::Camera* const c) noexcept
{
    camera_uniform_buffer = c->get_uniform_buffers()->get_buffer();
}