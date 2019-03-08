#include "gles2-buf-mesh.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-log.hpp"
#include "../engine/gles2-eng-engine.hpp"
#include "../gles2.hpp"

gearoenix::gles2::buffer::Mesh::Mesh(
    const std::shared_ptr<system::stream::Stream>& f,
    const std::shared_ptr<engine::Engine>& e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore> c)
    : render::buffer::Mesh(e)
{
    core::Count cnt;
    f->read(cnt);
    core::Count vsec = cnt * 12;
    std::vector<core::Real> vd((size_t)vsec);
    unsigned int vs = (unsigned int)(vsec * sizeof(core::Real));
    for (core::Count i = 0; i < vsec; ++i) {
        f->read(vd[(size_t)i]);
    }
    f->read(cnt);
    ic = (GLsizei)cnt;
    std::vector<GLushort> idata((size_t)cnt);
    for (core::Count i = 0; i < cnt; ++i)
        idata[(size_t)i] = static_cast<GLushort>(f->read<std::uint32_t>());
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
    const GLuint cvbo = vbo;
    const GLuint cibo = ibo;
    e->add_load_function([cvbo, cibo]() -> void {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &cvbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &cibo);
    });
    vbo = 0;
    ibo = 0;
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
