#ifndef GEAROENIX_GL_MESH_HPP
#define GEAROENIX_GL_MESH_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../render/mesh/gx-rnd-msh-manager.hpp"
#include "../render/mesh/gx-rnd-msh-mesh.hpp"
#include "gx-gl-types.hpp"

namespace gearoenix::gl {
struct Engine;
struct Mesh final : public render::mesh::Mesh {
    uint vertex_object = 0;
    uint vertex_buffer = 0;
    uint index_buffer = 0;
    sizei indices_count = 0;
    Engine& e;

    Mesh(Engine& e, const math::Aabb3<double>& box);
    ~Mesh() final;
    Mesh(const Mesh&) = delete;
};

struct MeshManager final : render::mesh::Manager {
    Engine& gl_e;

    explicit MeshManager(Engine& e);
    ~MeshManager() final;
    void build(
        std::string&& name,
        render::Vertices&& vertices,
        std::vector<std::uint32_t>&& indices,
        const math::Aabb3<double>& occlusion_box,
        core::job::EndCallerShared<render::mesh::Mesh>&& end_callback) final;
};
}

#endif
#endif
