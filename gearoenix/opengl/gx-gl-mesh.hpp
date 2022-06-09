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

    Mesh(Engine& e, const math::Aabb3<double>& box) noexcept;
    ~Mesh() noexcept final;
    Mesh(const Mesh&) = delete;
};

struct MeshManager final : render::mesh::Manager {
    explicit MeshManager(Engine& e) noexcept;
    ~MeshManager() noexcept final;
    [[nodiscard]] std::shared_ptr<render::mesh::Mesh> build(
        std::string&& name,
        std::vector<render::PbrVertex>&& vertices,
        std::vector<std::uint32_t>&& indices,
        math::Aabb3<double>&& occlusion_box,
        core::sync::EndCallerIgnored&& end_callback) noexcept final;
};
}

#endif
#endif
