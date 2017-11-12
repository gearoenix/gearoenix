#include "gles2-shd-shadeless-d2-matte-nonreflective-shadowless-opaque.hpp"
#ifdef USE_OPENGL_ES2
#include "../../system/sys-log.hpp"
#include "../buffer/gles2-buf-uniform.hpp"
#include "../gles2-engine.hpp"
#include "../pipeline/gles2-pip-pipeline.hpp"
#include "../texture/gles2-txt-2d.hpp"

gearoenix::gles2::shader::ShadelessD2MatteNonreflectiveShadowlessOpaque::Resources::Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u)
    : render::material::ShadelessD2MatteNonreflectiveShadowlessOpaque::Resources(e, pip, u)
{
}

void gearoenix::gles2::shader::ShadelessD2MatteNonreflectiveShadowlessOpaque::Resources::bind()
{
    render::material::ShadelessD2MatteNonreflectiveShadowlessOpaque::Uniform* data = reinterpret_cast<render::material::ShadelessD2MatteNonreflectiveShadowlessOpaque::Uniform*>(u->get_data());
    ShadelessD2MatteNonreflectiveShadowlessOpaque* shd = reinterpret_cast<ShadelessD2MatteNonreflectiveShadowlessOpaque*>(pip->get_shader());
    shd->use();
    shd->set_mvp(data->mvp.data());
    reinterpret_cast<texture::Texture2D*>(txt2d)->bind(GL_TEXTURE0);
}

gearoenix::gles2::shader::ShadelessD2MatteNonreflectiveShadowlessOpaque::ShadelessD2MatteNonreflectiveShadowlessOpaque(Engine* eng, std::shared_ptr<core::EndCaller> end)
    : Shader(eng, end)
{
    eng->add_load_function([this, end] {
        create_program();
        const std::string pvs = "precision highp sampler2D;\n"
                                "precision highp float;\n"
                                "attribute vec3 vertex;\n"
                                "attribute vec2 uv;\n"
                                "varying vec2 out_uv;\n"
                                "uniform mat4 mvp;\n"
                                "void main()\n"
                                "{\n"
                                "    out_uv = uv;\n"
                                "    gl_Position = mvp * vec4(vertex, 1.0);\n"
                                "}\n";
        const std::string pfs = "precision highp sampler2D;\n"
                                "precision highp float;\n"
                                "varying vec2 out_uv;\n"
                                "uniform sampler2D txt2d;\n"
                                "void main()\n"
                                "{\n"
                                "    gl_FragColor = vec4(texture2D(txt2d, out_uv).xyz, 1.0);\n"
                                "}\n";
        vtx_shd = add_shader_to_program(pvs, GL_VERTEX_SHADER);
        frg_shd = add_shader_to_program(pfs, GL_FRAGMENT_SHADER);
        run();
        vtx_att_ind = glGetAttribLocation(shader_program, "vertex");
        uv_att_ind = glGetAttribLocation(shader_program, "uv");
        mvp = get_uniform_location("mvp");
        txt2d = get_uniform_location("txt2d");
        (void)end;
    });
}

gearoenix::gles2::shader::ShadelessD2MatteNonreflectiveShadowlessOpaque::~ShadelessD2MatteNonreflectiveShadowlessOpaque()
{
    eng->add_load_function([this] {
        end_object(vtx_shd);
        end_object(frg_shd);
        end_program();
    });
}

void gearoenix::gles2::shader::ShadelessD2MatteNonreflectiveShadowlessOpaque::use()
{
    glUseProgram(shader_program);
    glEnableVertexAttribArray(vtx_att_ind);
    glEnableVertexAttribArray(uv_att_ind);
    ////////////////////////////////////////////////////////////
    glVertexAttribPointer(vtx_att_ind, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glVertexAttribPointer(uv_att_ind, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glUniform1i(txt2d, 0);
}

const std::vector<gearoenix::render::shader::stage::Id>& gearoenix::gles2::shader::ShadelessD2MatteNonreflectiveShadowlessOpaque::get_stages_ids() const
{
    return graphic_2_stage;
}

void gearoenix::gles2::shader::ShadelessD2MatteNonreflectiveShadowlessOpaque::set_mvp(const GLfloat* data)
{
    glUniformMatrix4fv(mvp, 1, GL_FALSE, data);
}
#endif