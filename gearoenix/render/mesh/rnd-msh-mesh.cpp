#include "rnd-msh-mesh.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../math/math-vertex.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../buffer/rnd-buf-manager.hpp"
#include "../buffer/rnd-buf-static.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../material/rnd-mat-material.hpp"
#include <vector>

gearoenix::render::mesh::Mesh::Mesh(const core::Id my_id, const Type::Id mesh_type_id)
    : core::asset::Asset(my_id, core::asset::Type::MESH)
    , mesh_type_id(mesh_type_id)
{
}

gearoenix::render::mesh::Mesh::Mesh(
    const core::Id my_id,
    const std::shared_ptr<system::stream::Stream>& f,
    const std::shared_ptr<engine::Engine>& e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c)
    : core::asset::Asset(my_id, core::asset::Type::MESH)
    , mesh_type_id(Type::BASIC)
{
    const core::Count mesh_count = f->read<core::Count>();
    std::vector<math::BasicVertex> vertices(mesh_count);
    for (math::BasicVertex& v : vertices) {
        v.read(f);
    }
    std::vector<std::uint32_t> indices;
    f->read(indices);
    radius = f->read<core::Real>();
    const std::shared_ptr<buffer::Manager>& bufmgr = e->get_buffer_manager();
    vertex_buffer = bufmgr->create_static(vertices, c);
    index_buffer = bufmgr->create_static(indices, c);
}

gearoenix::render::mesh::Mesh::~Mesh()
{
}

void gearoenix::render::mesh::Mesh::draw()
{
    GXUNIMPLEMENTED;
}

gearoenix::core::Real gearoenix::render::mesh::Mesh::get_radius() const
{
    return radius;
}

gearoenix::render::mesh::Type::Id gearoenix::render::mesh::Mesh::get_mesh_type_id() const
{
    return mesh_type_id;
}

const std::shared_ptr<gearoenix::render::buffer::Buffer>& gearoenix::render::mesh::Mesh::get_vertex_buffer() const
{
    return vertex_buffer;
}

const std::shared_ptr<gearoenix::render::buffer::Buffer>& gearoenix::render::mesh::Mesh::get_index_buffer() const
{
    return index_buffer;
}
