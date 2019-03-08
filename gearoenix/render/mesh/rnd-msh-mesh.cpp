#include "rnd-msh-mesh.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../buffer/rnd-buf-mesh.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../material/rnd-mat-material.hpp"

gearoenix::render::mesh::Mesh::Mesh(
    const Type::Id t,
    const core::Id my_id,
    const std::shared_ptr<system::stream::Stream>& f,
    const std::shared_ptr<engine::Engine>& e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore> c)
    : core::asset::Asset(my_id, core::asset::Type::MESH)
    , t(t)
{
    core::Count vertex_elements_count;
    f->read(vertex_elements_count);
    //buf = e->create_mesh((unsigned int)vertex_elements_count, f, c);
}

gearoenix::render::mesh::Mesh* gearoenix::render::mesh::Mesh::read(const core::Id my_id,
    const std::shared_ptr<system::stream::Stream>& f,
    const std::shared_ptr<engine::Engine>& e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore> c)
{
    const Type::Id t = f->read<Type::Id>();
    switch (t) {
    case Type::BASIC:
        return new Mesh(t, my_id, f, e, c);
    default:
        GXUNEXPECTED;
    }
    return nullptr;
}

gearoenix::render::mesh::Mesh::~Mesh()
{
}

void gearoenix::render::mesh::Mesh::bind()
{
    buf->bind();
}

void gearoenix::render::mesh::Mesh::draw()
{
    buf->draw();
}
