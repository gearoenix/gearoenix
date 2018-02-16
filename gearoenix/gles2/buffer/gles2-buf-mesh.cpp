#include "gles2-buf-mesh.hpp"
#ifdef USE_OPENGL_ES2
#include "../../system/file/sys-fl-file.hpp"
#include "../../system/sys-log.hpp"
#include "../gles2-engine.hpp"
#include "../gles2.hpp"

gearoenix::gles2::buffer::Mesh::Mesh(unsigned int vec, system::file::File* f, Engine* e, core::EndCaller<core::EndCallerIgnore> c)
    : render::buffer::Mesh(e)
{
    core::Count cnt;
    f->read(cnt);
    core::Count vsec = cnt * vec;
    std::vector<core::Real> vd((size_t)vsec);
    unsigned int vs = (unsigned int)(vsec * sizeof(core::Real));
    for (core::Count i = 0; i < vsec; ++i) {
        f->read(vd[(size_t)i]);
    }
    f->read(cnt);
    ic = (GLsizei)cnt;
    std::vector<GLushort> idata((size_t)cnt);
    for (core::Count i = 0; i < cnt; ++i)
        idata[(size_t)i] = static_cast<GLushort>(f->read<uint32_t>());
    unsigned int is = (unsigned int)(cnt * sizeof(GLushort));
    std::function<void()> todo = [this, vd, idata, vs, is, c] {
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vs, vd.data(), GL_STATIC_DRAW);
        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, is, idata.data(), GL_STATIC_DRAW);
    };
    e->add_load_function(todo);
}

gearoenix::gles2::buffer::Mesh::~Mesh()
{
    if (vbo == 0)
        return;
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &ibo);
    vbo = 0;
}

void gearoenix::gles2::buffer::Mesh::bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
}

void gearoenix::gles2::buffer::Mesh::draw()
{
    glDrawElements(GL_TRIANGLES, ic, GL_UNSIGNED_SHORT, 0);
}

#endif
