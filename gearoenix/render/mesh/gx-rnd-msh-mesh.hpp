#ifndef GEAROENIX_RENDER_MESH_MESH_HPP
#define GEAROENIX_RENDER_MESH_MESH_HPP
#include "../../math/gx-math-aabb.hpp"

namespace gearoenix::render::mesh {
struct Mesh {
    const math::Aabb3 box;

protected:
    Mesh(math::Aabb3&& box) noexcept;

public:
    virtual ~Mesh() noexcept;

    Mesh(Mesh&&) = delete;
    Mesh(const Mesh&) = delete;
    Mesh& operator=(Mesh&&) = delete;
    Mesh& operator=(const Mesh&) = delete;
};
}

#endif