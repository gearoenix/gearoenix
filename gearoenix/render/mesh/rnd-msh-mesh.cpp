#include "rnd-msh-mesh.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../math/math-vertex.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../buffer/rnd-buf-manager.hpp"
#include "../buffer/rnd-buf-static.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../material/rnd-mat-material.hpp"
#include <vector>

gearoenix::render::mesh::Mesh::Mesh(const core::Id my_id, const Type::Id mesh_type_id) noexcept
    : core::asset::Asset(my_id, core::asset::Type::MESH)
    , mesh_type_id(mesh_type_id)
{
}

gearoenix::render::mesh::Mesh::Mesh(
    const core::Id my_id,
    system::stream::Stream*const f,
    engine::Engine*const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : core::asset::Asset(my_id, core::asset::Type::MESH)
    , mesh_type_id(Type::BASIC)
{
    const auto mesh_count = f->read<core::Count>();
    std::vector<math::BasicVertex> vertices(mesh_count);
    for (math::BasicVertex& v : vertices) {
        v.read(f);
    }
    std::vector<std::uint32_t> indices;
    f->read(indices);
    const auto r = f->read<core::Real>();
    set_vertices(e, std::move(vertices), std::move(indices), r, c);
}

gearoenix::render::mesh::Mesh::Mesh(
    const core::Id my_id,
    std::vector<math::BasicVertex> vertices,
    std::vector<std::uint32_t> indices,
    const core::Real radius,
    engine::Engine*const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : core::asset::Asset(my_id, core::asset::Type::MESH)
    , mesh_type_id(Type::BASIC)
{
    set_vertices(e, std::move(vertices), std::move(indices), radius, c);
}

gearoenix::render::mesh::Mesh::~Mesh()noexcept
{
    radius = -1.0f;
    delete vertex_buffer;
    vertex_buffer = nullptr;
    delete index_buffer;
    index_buffer = nullptr;
}

void gearoenix::render::mesh::Mesh::set_vertices(
    engine::Engine*const e,
    std::vector<math::BasicVertex> vertices,
    std::vector<std::uint32_t> indices,
    const core::Real r,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    buffer::Manager* buf_mgr = e->get_buffer_manager();
    vertex_buffer = buf_mgr->create_static(std::move(vertices), c);
    index_buffer = buf_mgr->create_static(std::move(indices), c);
    radius = r;
}

gearoenix::core::Real gearoenix::render::mesh::Mesh::get_radius() const noexcept
{
    return radius;
}

gearoenix::render::mesh::Type::Id gearoenix::render::mesh::Mesh::get_mesh_type_id() const noexcept
{
    return mesh_type_id;
}

const gearoenix::render::buffer::Buffer* gearoenix::render::mesh::Mesh::get_vertex_buffer() const noexcept
{
    return vertex_buffer;
}

gearoenix::render::buffer::Buffer* gearoenix::render::mesh::Mesh::get_vertex_buffer() noexcept
{
    return vertex_buffer;
}

const gearoenix::render::buffer::Buffer* gearoenix::render::mesh::Mesh::get_index_buffer() const noexcept
{
    return index_buffer;
}

gearoenix::render::buffer::Buffer* gearoenix::render::mesh::Mesh::get_index_buffer() noexcept
{
    return index_buffer;
}
