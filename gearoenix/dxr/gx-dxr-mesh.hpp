#pragma once

#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_DXR_ENABLED
#include "../core/ecs/gx-cr-ecs-component.hpp"
#include "../render/mesh/gx-rnd-msh-builder.hpp"
#include "../render/mesh/gx-rnd-msh-manager.hpp"
#include "gx-dxr-buffer.hpp"
#include "gx-dxr-loader.hpp"

namespace gearoenix::dxr {
struct Mesh final : public core::ecs::Component {
    GX_GET_REFC_PRV(std::shared_ptr<GpuBuffer>, vb)
    GX_GET_REFC_PRV(std::shared_ptr<GpuBuffer>, ib)
    GX_GET_CVAL_PRV(UINT, indices_count)
    GX_GET_REFC_PRV(D3D12_VERTEX_BUFFER_VIEW, vv)
    GX_GET_REFC_PRV(D3D12_INDEX_BUFFER_VIEW, iv)

    Mesh(
        std::shared_ptr<GpuBuffer>&& vb,
        std::shared_ptr<GpuBuffer>&& ib,
        UINT vertex_size,
        UINT vertices_size,
        UINT indices_count) noexcept;
    ~Mesh() noexcept final;
    Mesh(Mesh&&) noexcept;
    Mesh(const Mesh&) = delete;
    Mesh& operator=(Mesh&&) = delete;
    Mesh& operator=(const Mesh&) = delete;
};

struct MaterialBuffer final : public core::ecs::Component {
    UniformBuffer uniform;

    MaterialBuffer(
        Engine& e,
        UINT buffer_size,
        LPCWSTR entity_name) noexcept;
    ~MaterialBuffer() noexcept final;
    MaterialBuffer(MaterialBuffer&&) noexcept;
    MaterialBuffer(const MaterialBuffer&) = delete;
    MaterialBuffer& operator=(MaterialBuffer&&) = delete;
    MaterialBuffer& operator=(const MaterialBuffer&) = delete;
};

struct MeshBuilder final : render::mesh::Builder {
    friend struct MeshManager;

private:
    Engine& e;

    MeshBuilder(
        Engine& e,
        const std::string& name,
        math::Aabb3&& occlusion_box) noexcept;

    void set_material_type_index(const std::type_index& material_type) noexcept final;

public:
    ~MeshBuilder() noexcept final;
};

struct MeshManager final : render::mesh::Manager {
    explicit MeshManager(Engine& e) noexcept;
    ~MeshManager() noexcept final;
    [[nodiscard]] std::shared_ptr<render::mesh::Builder> build(
        const std::string& name,
        const std::vector<render::PbrVertex>& vertices,
        const std::vector<std::uint32_t>& indices,
        math::Aabb3&& occlusion_box,
        const core::sync::EndCallerIgnored& c) noexcept final;
};
}

#endif