#include "gx-rnd-msh-mesh.hpp"
#include "../material/gx-rnd-mat-material.hpp"

gearoenix::render::mesh::Mesh::Mesh(
    std::shared_ptr<Buffer>&& buffer, std::shared_ptr<material::Material>&& material)
    : buffer(std::move(buffer))
    , bound_material(std::move(material))
{
}

gearoenix::render::mesh::Mesh::~Mesh() = default;

void gearoenix::render::mesh::Mesh::set_material(std::shared_ptr<material::Material>&& material)
{
    bound_material = std::move(material);
}