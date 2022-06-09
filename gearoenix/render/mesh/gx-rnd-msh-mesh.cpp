#include "gx-rnd-msh-mesh.hpp"

gearoenix::render::mesh::Mesh::Mesh(const math::Aabb3<double>& box) noexcept
    : box(box)
{
}

gearoenix::render::mesh::Mesh::~Mesh() noexcept = default;