#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../render/mesh/gx-rnd-msh-buffer.hpp"
#include "../render/mesh/gx-rnd-msh-manager.hpp"
#include "../render/mesh/gx-rnd-msh-mesh.hpp"
#include "gx-gl-types.hpp"

namespace gearoenix::gl::material {
struct Material;
}
namespace gearoenix::gl {
struct Buffer final : render::mesh::Buffer {
    GX_GET_VAL_PRV(uint, vertex_object, 0);
    GX_GET_VAL_PRV(uint, vertex_buffer, 0);
    GX_GET_VAL_PRV(uint, index_buffer, 0);
    GX_GET_VAL_PRV(sizei, indices_count, 0);

public:
    explicit Buffer(const math::Aabb3<double>& box);
    static void construct(
        std::string&& name,
        render::Vertices&& vertices,
        std::vector<std::uint32_t>&& indices,
        const math::Aabb3<double>& occlusion_box,
        core::job::EndCallerShared<render::mesh::Buffer>&& end_callback);
    ~Buffer() override;
};

struct Mesh final : render::mesh::Mesh {
    GX_GET_CREF_PRV(std::shared_ptr<gl::Buffer>, gl_buffer);
    GX_GET_CREF_PRV(std::shared_ptr<gl::material::Material>, gl_material);
    GX_GET_VAL_PRV(uint, cached_vertex_object, 0);
    GX_GET_VAL_PRV(uint, cached_vertex_buffer, 0);
    GX_GET_VAL_PRV(uint, cached_index_buffer, 0);
    GX_GET_VAL_PRV(sizei, cached_indices_count, 0);

public:
    Mesh(std::shared_ptr<render::mesh::Buffer>&& buffer, std::shared_ptr<render::material::Material>&& material);
    static void construct(
        std::shared_ptr<render::mesh::Buffer>&& buffer,
        std::shared_ptr<render::material::Material>&& material,
        const core::job::EndCallerShared<render::mesh::Mesh>& end_callback);
    ~Mesh() override;
    Mesh(const Mesh&) = delete;
};

struct MeshManager final : render::mesh::Manager {
    MeshManager();
    ~MeshManager() override;
    void build(
        std::string&& name,
        render::Vertices&& vertices,
        std::vector<std::uint32_t>&& indices,
        const math::Aabb3<double>& occlusion_box,
        core::job::EndCallerShared<render::mesh::Buffer>&& end_callback) override;
    void build(
        std::shared_ptr<render::mesh::Buffer>&& buffer,
        std::shared_ptr<render::material::Material>&& material,
        core::job::EndCallerShared<render::mesh::Mesh>&& end_callback) override;
};
}

#endif