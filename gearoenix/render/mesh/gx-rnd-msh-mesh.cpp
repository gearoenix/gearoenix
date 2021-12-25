#include "gx-rnd-msh-mesh.hpp"

gearoenix::render::mesh::Mesh::Mesh(math::Aabb3&& box) noexcept
    : box(std::move(box))
{
}

gearoenix::render::mesh::Mesh::~Mesh() noexcept = default;