#include "gx-rnd-msh-buffer.hpp"

gearoenix::render::mesh::Buffer::Buffer(const math::Aabb3<double>& box)
    : box(box)
{
}

gearoenix::render::mesh::Buffer::~Buffer() = default;
