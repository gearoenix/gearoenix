#include "rnd-msh-mesh.hpp"
#include "../../core/cr-end-caller.hpp"
#include "../../system/sys-file.hpp"
#include "../buffer/rnd-buf-mesh.hpp"
#include "../material/rnd-mat-material.hpp"
#include "../rnd-engine.hpp"

gearoenix::render::mesh::Mesh::Mesh(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c)
{
    core::Count vertex_elements_count;
    f->read(vertex_elements_count);
    buf = e->create_mesh(vertex_elements_count, f, c);
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
