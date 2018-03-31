#include "gles2-shd-font-colored.hpp"
#ifdef USE_OPENGL_ES2
#include "../../system/sys-log.hpp"
#include "../buffer/gles2-buf-uniform.hpp"
#include "../gles2-engine.hpp"
#include "../pipeline/gles2-pip-pipeline.hpp"
#include "../texture/gles2-txt-2d.hpp"

gearoenix::gles2::shader::FontColored::Resources::Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u)
    : render::material::FontColored::Resources(e, pip, u)
{
}

void gearoenix::gles2::shader::FontColored::Resources::bind()
{
    render::material::FontColored::Uniform* data = reinterpret_cast<render::material::FontColored::Uniform*>(u->get_data());
    FontColored* shd = reinterpret_cast<FontColored*>(pip->get_shader());
    shd->use();
    shd->set_mvp(data->mvp.data());
    shd->set_color(data->color.data());
    reinterpret_cast<texture::Texture2D*>(txt2d)->bind(GL_TEXTURE0);
}

gearoenix::gles2::shader::FontColored::FontColored(core::Id my_id, Engine* eng, core::sync::EndCaller<core::sync::EndCallerIgnore> end)
    : Shader(my_id, eng, end)
{
    eng->add_load_function([this, end] {
        create_program();
        const std::string pvs = "#version 100\n"
                                "precision highp sampler2D;\n"
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
        const std::string pfs = "#version 100\n"
                                "precision highp sampler2D;\n"
                                "precision highp float;\n"
                                "varying vec2 out_uv;\n"
                                "uniform vec4 color;\n"
                                "uniform sampler2D txt2d;\n"
                                "void main()\n"
                                "{\n"
                                "    vec4 txt_color = texture2D(txt2d, out_uv);\n"
                                "    gl_FragColor = vec4(txt_color.xyz * color.xyz, txt_color.x * color.w);\n"
                                "}\n";
        vtx_shd = add_shader_to_program(pvs, GL_VERTEX_SHADER);
        frg_shd = add_shader_to_program(pfs, GL_FRAGMENT_SHADER);
        run();
        vtx_att_ind = glGetAttribLocation(shader_program, "vertex");
        uv_att_ind = glGetAttribLocation(shader_program, "uv");
        mvp = get_uniform_location("mvp");
        txt2d = get_uniform_location("txt2d");
        color = get_uniform_location("color");
        (void)end;
    });
}

gearoenix::gles2::shader::FontColored::~FontColored()
{
    if (vtx_shd == 0)
        return;
    const GLuint c_vtx_shd = vtx_shd;
    const GLuint c_frg_shd = frg_shd;
    const GLuint c_shader_program = shader_program;
    eng->add_load_function([c_vtx_shd, c_frg_shd, c_shader_program] {
        end_object(c_vtx_shd);
        end_object(c_frg_shd);
        end_program(c_shader_program);
    });
    vtx_shd = 0;
    frg_shd = 0;
    shader_program = 0;
}

void gearoenix::gles2::shader::FontColored::use()
{
    glUseProgram(shader_program);
    glEnableVertexAttribArray(vtx_att_ind);
    glEnableVertexAttribArray(uv_att_ind);
    ////////////////////////////////////////////////////////////
    glVertexAttribPointer(vtx_att_ind, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glVertexAttribPointer(uv_att_ind, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glUniform1i(txt2d, 0);
}

const std::vector<gearoenix::render::shader::stage::Id>& gearoenix::gles2::shader::FontColored::get_stages_ids() const
{
    return graphic_2_stage;
}

void gearoenix::gles2::shader::FontColored::set_mvp(const GLfloat* data)
{
    glUniformMatrix4fv(mvp, 1, GL_FALSE, data);
}

void gearoenix::gles2::shader::FontColored::set_color(const GLfloat* data)
{
    glUniform4fv(color, 1, data);
}
#endif
