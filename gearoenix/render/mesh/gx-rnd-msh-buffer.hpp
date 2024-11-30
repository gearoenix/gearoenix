#ifndef GEAROENIX_RENDER_MESH_BUFFER_HPP
#define GEAROENIX_RENDER_MESH_BUFFER_HPP
#include "../../math/gx-math-aabb.hpp"

namespace gearoenix::render::material {
struct Material;
}

namespace gearoenix::render::mesh {
struct Buffer {
    constexpr static std::uint32_t MAX_COUNT = 8192;

    GX_GET_REFC_PRT(math::Aabb3<double>, box);

    explicit Buffer(const math::Aabb3<double>& box);

public:
    virtual ~Buffer();

    Buffer(Buffer&&) = delete;
    Buffer(const Buffer&) = delete;
    Buffer& operator=(Buffer&&) = delete;
    Buffer& operator=(const Buffer&) = delete;
};
}

#endif