#include "gx-mtl-mesh.hpp"
#ifdef GX_RENDER_METAL_ENABLED
#import "gx-mtl-engine.hpp"
#import "gx-mtl-heap.hpp"
#import "gx-mtl-uploader.hpp"
#import "shaders/gx-mtl-shd-common.hpp"

gearoenix::metal::Mesh::Mesh(
    id<MTLBuffer> vb,
    id<MTLBuffer> ib,
    math::Aabb3<double>&& box,
    const NSUInteger vertex_size,
    const NSUInteger vertices_size,
    const NSUInteger indices_count)
    : render::mesh::Mesh(std::move(box))
    , vertex_buffer(vb)
    , index_buffer(ib)
    , indices_count(indices_count)
{
}

gearoenix::metal::Mesh::~Mesh()
{
    [vertex_buffer release];
    [index_buffer release];
}

gearoenix::metal::MeshManager::MeshManager(Engine& e)
    : render::mesh::Manager(e)
{
}

gearoenix::metal::MeshManager::~MeshManager() = default;

std::shared_ptr<gearoenix::render::mesh::Mesh> gearoenix::metal::MeshManager::build(
    std::string&& name,
    std::vector<render::PbrVertex>&& vertices,
    std::vector<std::uint32_t>&& indices,
    math::Aabb3<double>&& occlusion_box,
    core::job::EndCaller&& c)
{
    auto& eng = dynamic_cast<Engine&>(e);
    auto* const gpu_heap = eng.get_heap_manager()->gpu;
    const auto vsz = static_cast<NSUInteger>(vertices.size() * sizeof(render::PbrVertex));
    auto vb = [gpu_heap newBufferWithLength:vsz options:MTLResourceStorageModePrivate];
    const auto isz = static_cast<NSUInteger>(indices.size() * sizeof(std::uint32_t));
    auto ib = [gpu_heap newBufferWithLength:isz options:MTLResourceStorageModePrivate];

#ifdef GEAROENIX_METAL_RESOURCE_NAMING
    vb.label = [NSString stringWithFormat:@"Gearoenix-VertexBuffer-%s", name.c_str()];
    ib.label = [NSString stringWithFormat:@"Gearoenix-IndexBuffer-%s", name.c_str()];
#endif

    auto m = std::make_shared<Mesh>(
        vb, ib, std::move(occlusion_box),
        static_cast<NSUInteger>(sizeof(render::PbrVertex)),
        vsz, static_cast<NSUInteger>(indices.size()));
    core::job::EndCaller end([c, m] { });
    auto* const u = eng.get_uploader();
    u->upload(vb, vertices, core::job::EndCaller(end));
    u->upload(ib, indices, std::move(end));
    return m;
}

#endif
