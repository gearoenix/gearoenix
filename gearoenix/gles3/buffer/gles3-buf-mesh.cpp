#include "gles3-buf-mesh.hpp"
#ifdef USE_OPENGL_ES3
#include "../../system/stream/sys-fl-file.hpp"
#include "../../system/sys-log.hpp"
#include "../gles3-engine.hpp"
#include "../gles3.hpp"

gearoenix::gles3::buffer::Mesh::Mesh(unsigned int vec, system::stream::Stream* f, Engine* e, core::EndCaller<core::EndCallerIgnore> c)
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
    ic = (GLuint)cnt;
    std::vector<GLuint> idata((size_t)cnt);
    for (core::Count i = 0; i < cnt; ++i)
        idata[(size_t)i] = (GLuint)f->read<std::uint32_t>();
    unsigned int is = (unsigned int)(cnt * sizeof(GLuint));
    std::function<void()> todo = [this, vd, idata, vs, is, c, vec] {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vs, vd.data(), GL_STATIC_DRAW);
        for (int rvec = vec, vapi = 0; rvec > 0; rvec -= 3, ++vapi) {
            glEnableVertexAttribArray(vapi);
            glVertexAttribPointer(vapi, rvec > 2 ? 3 : rvec, GL_FLOAT, GL_FALSE, vec * sizeof(GLfloat), (void*)(vapi * 3 * sizeof(GLfloat)));
        }
        glBindVertexArray(0);
        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, is, idata.data(), GL_STATIC_DRAW);
    };
    e->add_load_function(todo);
}

gearoenix::gles3::buffer::Mesh::~Mesh()
{
    if (vbo == 0)
        return;
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vao);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &ibo);
    vbo = 0;
}

void gearoenix::gles3::buffer::Mesh::bind()
{
    //    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
}

void gearoenix::gles3::buffer::Mesh::draw()
{
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glDrawElements(GL_TRIANGLES, ic, GL_UNSIGNED_INT, 0);
}

#endif
