#include "gx-mtl-mesh.hpp"
#ifdef GX_RENDER_METAL_ENABLED
#import "gx-mtl-engine.hpp"
#import "gx-mtl-uploader.hpp"

gearoenix::metal::Mesh::Mesh(
    id<MTLBuffer>&& vb,
    id<MTLBuffer>&& ib,
    math::Aabb3<double>&& box,
    const NSUInteger vertex_size,
    const NSUInteger vertices_size,
    const NSUInteger indices_count) noexcept
    : render::mesh::Mesh(std::move(box))
    , vertex_buffer(std::move(vb))
    , index_buffer(std::move(ib))
    , indices_count(indices_count) {}

gearoenix::metal::Mesh::~Mesh() noexcept
{
    [vertex_buffer release];
    [index_buffer release];
}

gearoenix::metal::MeshManager::MeshManager(Engine& e) noexcept
    : render::mesh::Manager(e)
{
}

gearoenix::metal::MeshManager::~MeshManager() noexcept = default;

std::shared_ptr<gearoenix::render::mesh::Mesh> gearoenix::metal::MeshManager::build(
    std::string&& name,
    std::vector<render::PbrVertex>&& vertices,
    std::vector<std::uint32_t>&& indices,
    math::Aabb3<double>&& occlusion_box,
    core::sync::EndCallerIgnored&& c) noexcept
{
    auto& eng = dynamic_cast<Engine&>(e);
    auto* const d = eng.get_device();
    const auto vsz = static_cast<NSUInteger>(vertices.size() * sizeof(render::PbrVertex));
    auto vb = [d newBufferWithLength:vsz options:MTLResourceStorageModePrivate];
    vb.label = [NSString stringWithFormat:@"Gearoenix-VertexBuffer-%s", name.c_str()];
    const auto isz = static_cast<NSUInteger>(indices.size() * sizeof(std::uint32_t));
    auto ib = [d newBufferWithLength:isz options:MTLResourceStorageModePrivate];
    ib.label = [NSString stringWithFormat:@"Gearoenix-IndexBuffer-%s", name.c_str()];
    auto m = std::make_shared<Mesh>(
        std::move(vb),
        std::move(ib),
        std::move(occlusion_box),
        static_cast<NSUInteger>(sizeof(render::PbrVertex)),
        vsz,
        static_cast<NSUInteger>(indices.size()));
    core::sync::EndCallerIgnored end([c, m]{});
    auto*const u = eng.get_uploader();
    u->upload(vb, vertices, core::sync::EndCallerIgnored(end));
    u->upload(ib, indices, std::move(end));
    return m;
}

#endif
