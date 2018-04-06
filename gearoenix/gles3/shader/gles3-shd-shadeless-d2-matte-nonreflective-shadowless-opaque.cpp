#include "gles3-shd-shadeless-d2-matte-nonreflective-shadowless-opaque.hpp"
#ifdef USE_OPENGL_ES3
#include "../../system/sys-log.hpp"
#include "../buffer/gles3-buf-uniform.hpp"
#include "../gles3-engine.hpp"
#include "../pipeline/gles3-pip-pipeline.hpp"
#include "../texture/gles3-txt-2d.hpp"

gearoenix::gles3::shader::ShadelessD2MatteNonreflectiveShadowlessOpaque::Resources::Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u)
    : render::material::ShadelessD2MatteNonreflectiveShadowlessOpaque::Resources(e, pip, u)
{
}

void gearoenix::gles3::shader::ShadelessD2MatteNonreflectiveShadowlessOpaque::Resources::bind()
{
    render::material::ShadelessD2MatteNonreflectiveShadowlessOpaque::Uniform* data = static_cast<render::material::ShadelessD2MatteNonreflectiveShadowlessOpaque::Uniform*>(u->get_data());
    ShadelessD2MatteNonreflectiveShadowlessOpaque* shd = static_cast<ShadelessD2MatteNonreflectiveShadowlessOpaque*>(pip->get_shader());
    shd->use();
    shd->set_mvp(data->mvp.data());
    static_cast<texture::Texture2D*>(txt2d)->bind(GL_TEXTURE0);
}

gearoenix::gles3::shader::ShadelessD2MatteNonreflectiveShadowlessOpaque::ShadelessD2MatteNonreflectiveShadowlessOpaque(core::Id my_id, Engine* eng, core::sync::EndCaller<core::sync::EndCallerIgnore> end)
    : Shader(my_id, eng, end)
{
    eng->add_load_function([this, end] {
        create_program();
        const std::string pvs = "#version 330\n"
                                "layout(location = 0) in vec3 vertex;\n"
                                "layout(location = 1) in vec2 uv;\n"
                                "out vec2 out_uv;\n"
                                "uniform mat4 mvp;\n"
                                "void main()\n"
                                "{\n"
                                "    out_uv = uv;\n"
                                "    gl_Position = mvp * vec4(vertex, 1.0);\n"
                                "}\n";
        const std::string pfs = "#version 330\n"
                                "in vec2 out_uv;\n"
                                "out vec4 frag_color;\n"
                                "uniform sampler2D txt2d;\n"
                                "void main()\n"
                                "{\n"
                                "    frag_color = texture(txt2d, out_uv);\n"
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

gearoenix::gles3::shader::ShadelessD2MatteNonreflectiveShadowlessOpaque::~ShadelessD2MatteNonreflectiveShadowlessOpaque()
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

void gearoenix::gles3::shader::ShadelessD2MatteNonreflectiveShadowlessOpaque::use()
{
    glUseProgram(shader_program);
    glUniform1i(txt2d, 0);
}

const std::vector<gearoenix::render::shader::stage::Id>& gearoenix::gles3::shader::ShadelessD2MatteNonreflectiveShadowlessOpaque::get_stages_ids() const
{
    return graphic_2_stage;
}

void gearoenix::gles3::shader::ShadelessD2MatteNonreflectiveShadowlessOpaque::set_mvp(const GLfloat* data)
{
    glUniformMatrix4fv(mvp, 1, GL_FALSE, data);
}
#endif
