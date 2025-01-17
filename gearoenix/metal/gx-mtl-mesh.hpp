#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_METAL_ENABLED
#import "../render/mesh/gx-rnd-msh-manager.hpp"
#import "../render/mesh/gx-rnd-msh-mesh.hpp"
#import <Metal/MTLBuffer.h>

namespace gearoenix::metal {
struct Engine;
struct Mesh final : public render::mesh::Mesh {
    const id<MTLBuffer> vertex_buffer;
    const id<MTLBuffer> index_buffer;
    const NSUInteger indices_count;

    Mesh(
        id<MTLBuffer> vb, id<MTLBuffer> ib, math::Aabb3<double>&& box,
        NSUInteger vertex_size, NSUInteger vertices_size, NSUInteger indices_count);
    ~Mesh() final;
    Mesh(const Mesh&) = delete;
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