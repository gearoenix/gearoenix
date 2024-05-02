#ifndef GEAROENIX_GL_SUBMISSION_MESH_HPP
#define GEAROENIX_GL_SUBMISSION_MESH_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../math/gx-math-aabb.hpp"
#include "../../math/gx-math-matrix-4d.hpp"
#include "../gx-gl-types.hpp"
#include <array>

namespace gearoenix::render::material {
struct Material;
}

namespace gearoenix::gl {
struct Mesh;
}

namespace gearoenix::gl::material {
struct Material;
}

namespace gearoenix::gl::submission {
struct Mesh final {
    material::Material* material = nullptr;
    render::material::Material* render_material = nullptr;
    uint vertex_object = 0;
    sizei indices_count = 0;

    explicit Mesh(const gl::Mesh& m);
};
}
#endif
#endif