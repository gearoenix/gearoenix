#ifndef GEAROENIX_RENDER_MESH_MESH_HPP
#define GEAROENIX_RENDER_MESH_MESH_HPP
#include "../../math/gx-math-aabb.hpp"

namespace gearoenix::render::material {
struct Material;
}

namespace gearoenix::render::mesh {
struct Buffer;
struct Mesh {
    static constexpr std::size_t MAX_COUNT = 8192;

    GX_GET_CREF_PRT(std::shared_ptr<Buffer>, buffer);
    GX_GET_CREF_PRT(std::shared_ptr<material::Material>, bound_material);

    Mesh(std::shared_ptr<Buffer>&& buffer, std::shared_ptr<material::Material>&& material);

public:
    virtual ~Mesh();

    Mesh(Mesh&&) = delete;
    Mesh(const Mesh&) = delete;
    Mesh& operator=(Mesh&&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    virtual void set_material(std::shared_ptr<material::Material>&& material);
};
}

#endif