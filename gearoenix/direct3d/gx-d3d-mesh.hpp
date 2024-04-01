#pragma once

#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_DIRECT3D_ENABLED
#include "../render/mesh/gx-rnd-msh-manager.hpp"
#include "../render/mesh/gx-rnd-msh-mesh.hpp"
#include "gx-d3d-loader.hpp"

namespace gearoenix::d3d {
struct GpuBuffer;
struct Engine;
struct Mesh final : public render::mesh::Mesh {
    const std::shared_ptr<GpuBuffer> vb;
    const std::shared_ptr<GpuBuffer> ib;
    const UINT indices_count;
    const D3D12_VERTEX_BUFFER_VIEW vv;
    const D3D12_INDEX_BUFFER_VIEW iv;

    Mesh(
        std::shared_ptr<GpuBuffer>&& vb,
        std::shared_ptr<GpuBuffer>&& ib,
        math::Aabb3<double>&& box,
        UINT vertex_size,
        UINT vertices_size,
        UINT indices_count);
    ~Mesh() final;
};

struct MeshManager final : render::mesh::Manager {
    explicit MeshManager(Engine& e);
    ~MeshManager() final;
    [[nodiscard]] std::shared_ptr<render::mesh::Mesh> build(
        std::string&& name,
        std::vector<render::PbrVertex>&& vertices,
        std::vector<std::uint32_t>&& indices,
        math::Aabb3<double>&& occlusion_box,
        core::job::EndCaller&& end_callback) final;
};
}

#endif