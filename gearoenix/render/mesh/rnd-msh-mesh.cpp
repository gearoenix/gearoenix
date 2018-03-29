#include "rnd-msh-mesh.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../buffer/rnd-buf-mesh.hpp"
#include "../material/rnd-mat-material.hpp"
#include "../rnd-engine.hpp"

gearoenix::render::mesh::Mesh::Mesh(core::Id my_id, system::stream::Stream* f, Engine* e, core::sync::EndCaller<core::sync::EndCallerIgnore> c)
    : core::asset::Asset(my_id, core::asset::Asset::AssetType::MESH)
{
    core::Count vertex_elements_count;
    f->read(vertex_elements_count);
    buf = e->create_mesh((unsigned int)vertex_elements_count, f, c);
}

gearoenix::render::mesh::Mesh* gearoenix::render::mesh::Mesh::read(core::Id my_id, system::stream::Stream* f, Engine* e, core::sync::EndCaller<core::sync::EndCallerIgnore> c)
{
    core::Id t;
    f->read(t);
    switch (t) {
    case Geo::BASIC:
        return new Mesh(my_id, f, e, c);
    default:
        UNEXPECTED;
    }
    return nullptr;
}

gearoenix::render::mesh::Mesh::~Mesh()
{
    delete buf;
}

void gearoenix::render::mesh::Mesh::bind()
{
    buf->bind();
}

void gearoenix::render::mesh::Mesh::draw()
{
    buf->draw();
}
