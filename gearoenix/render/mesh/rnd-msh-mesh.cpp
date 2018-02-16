#include "rnd-msh-mesh.hpp"
#include "../../core/cr-end-caller.hpp"
#include "../../system/file/sys-fl-file.hpp"
#include "../buffer/rnd-buf-mesh.hpp"
#include "../material/rnd-mat-material.hpp"
#include "../rnd-engine.hpp"

gearoenix::render::mesh::Mesh::Mesh(system::file::File* f, Engine* e, core::EndCaller<core::EndCallerIgnore> c)
{
    core::Count vertex_elements_count;
    f->read(vertex_elements_count);
    buf = e->create_mesh((unsigned int)vertex_elements_count, f, c);
}

gearoenix::render::mesh::Mesh* gearoenix::render::mesh::Mesh::read(system::file::File* f, Engine* e, core::EndCaller<core::EndCallerIgnore> c)
{
    core::Id t;
    f->read(t);
    switch (t) {
    case 1:
        return new Mesh(f, e, c);
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
