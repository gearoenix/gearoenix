#include "rnd-msh-mesh.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../math/math-vertex.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../buffer/rnd-buf-manager.hpp"
#include "../buffer/rnd-buf-static.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../material/rnd-mat-material.hpp"
#include <vector>

gearoenix::render::mesh::Mesh::Mesh(const core::Id my_id, const Type t) noexcept
    : core::asset::Asset(my_id, core::asset::Type::Mesh)
    , mesh_type(t)
{
}

gearoenix::render::mesh::Mesh::Mesh(
    const core::Id my_id,
    system::stream::Stream* const f,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : core::asset::Asset(my_id, core::asset::Type::Mesh)
    , mesh_type(Type::Basic)
{
    const auto mesh_count = f->read<core::Count>();
    std::vector<math::BasicVertex> vertices(mesh_count);
    for (math::BasicVertex& v : vertices) {
        v.read(f);
    }
    std::vector<std::uint32_t> indices;
    f->read(indices);
    const auto r = f->read<core::Real>();
    GXTODO // make sure the blender plugin implemented this
        box.read(f);
    set_vertices(e, vertices, indices, box, c);
}

gearoenix::render::mesh::Mesh::Mesh(
    const core::Id my_id,
    const std::vector<math::BasicVertex>& vertices,
    const std::vector<std::uint32_t>& indices,
    const math::Aabb3& b,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : core::asset::Asset(my_id, core::asset::Type::Mesh)
    , mesh_type(Type::Basic)
{
    set_vertices(e, vertices, indices, b, c);
}

gearoenix::render::mesh::Mesh::~Mesh() noexcept
{
    vertex_buffer = nullptr;
    index_buffer = nullptr;
}

void gearoenix::render::mesh::Mesh::set_vertices(
    engine::Engine* const e,
    const std::vector<math::BasicVertex>& vertices,
    const std::vector<std::uint32_t>& indices,
    const math::Aabb3& b,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    auto* const buf_mgr = e->get_buffer_manager();
    vertex_buffer = std::unique_ptr<buffer::Buffer>(buf_mgr->create_static(vertices, c));
    index_buffer = std::unique_ptr<buffer::Buffer>(buf_mgr->create_static(indices, c));
    box = b;
}