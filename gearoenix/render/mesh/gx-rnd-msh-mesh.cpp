#include "gx-rnd-msh-mesh.hpp"

gearoenix::render::mesh::Mesh::Mesh(const math::Aabb3<double>& box)
    : box(box)
{
}

gearoenix::render::mesh::Mesh::~Mesh() = default;