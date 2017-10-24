#include "gles2-buf-mesh.hpp"
#ifdef USE_OPENGL_ES2
#include "../../system/sys-file.hpp"
#include "../../system/sys-log.hpp"
#include "../gles2-engine.hpp"

gearoenix::gles2::buffer::Mesh::Mesh(unsigned int vec, system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c)
{
    core::Count cnt;
    f->read(cnt);
    core::Count vsec = cnt * vec;
    std::vector<core::Real> vd(vsec);
    unsigned int vds = vsec * sizeof(core::Real);
    unsigned int vs = vds;
    for (core::Count i = 0; i < vsec; ++i)
        f->read(vd[i]);
    f->read(cnt);
    ic = cnt;
    std::vector<GLushort> idata(cnt);
    for (core::Count i = 0; i < cnt; ++i)
        idata[i] = static_cast<GLushort>(f->read<uint32_t>());
    unsigned int ids = cnt * sizeof(GLushort);
    unsigned int is = ids;
    std::function<void()> todo = [this, vd, idata, vs, is, c] {
        for (core::Real r : vd) {
            LOGI(r);
        }
        LOGI("***********************************************");
        for (GLushort i : idata) {
            LOGI(i);
        }

        const GLfloat vertices[] = {
            0.0f, 1.0f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 2.0f,
            1.0f, 0.0f, -0.5f, 0.0f, 0.0f, 1.0f, 2.0f, 0.0f,
            -1.0f, 0.0f, -0.5f, 0.0f, 0.0f, 1.0f, -2.0f, 0.0f,
        };
        const GLushort indices[] = {
            0, 2, 1,
        };
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        ic = 3;

        //        glGenBuffers(1, &vbo);
        //        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        //        glBufferData(GL_ARRAY_BUFFER, vs, vd.data(), GL_STATIC_DRAW);
        //        glGenBuffers(1, &ibo);
        //        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        //        glBufferData(GL_ELEMENT_ARRAY_BUFFER, is, idata.data(), GL_STATIC_DRAW);
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
    glDrawElements(GL_TRIANGLES, ic, GL_UNSIGNED_INT, 0);
}

#endif
