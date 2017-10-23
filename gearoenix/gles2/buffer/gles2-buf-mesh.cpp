#include "gles2-buf-mesh.hpp"
#ifdef USE_OPENGL_ES2
#include "../../system/sys-log.hpp"
#include "../gles2-engine.hpp"

gearoenix::gles2::buffer::Mesh::Mesh(unsigned int vec, system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c)
    : render::buffer::Mesh(vec, f, e, c)
{
    UNIMPLEMENTED;
}

gearoenix::gles2::buffer::Mesh::~Mesh()
{
    UNIMPLEMENTED;
}

void gearoenix::gles2::buffer::Mesh::bind()
{
    UNIMPLEMENTED;
}

void gearoenix::gles2::buffer::Mesh::draw()
{
    UNIMPLEMENTED;
}

#endif
