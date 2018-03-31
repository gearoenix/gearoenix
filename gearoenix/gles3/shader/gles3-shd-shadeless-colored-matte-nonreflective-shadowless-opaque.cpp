#include "gles3-shd-shadeless-colored-matte-nonreflective-shadowless-opaque.hpp"
#ifdef USE_OPENGL_ES3
#include "../../system/sys-log.hpp"
#include "../buffer/gles3-buf-uniform.hpp"
#include "../gles3-engine.hpp"
#include "../pipeline/gles3-pip-pipeline.hpp"
#include "../texture/gles3-txt-cube.hpp"

gearoenix::gles3::shader::ShadelessColoredMatteNonreflectiveShadowlessOpaque::Resources::Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u)
    : render::material::ShadelessColoredMatteNonreflectiveShadowlessOpaque::Resources(e, pip, u)
{
}

void gearoenix::gles3::shader::ShadelessColoredMatteNonreflectiveShadowlessOpaque::Resources::bind()
{
    render::material::ShadelessColoredMatteNonreflectiveShadowlessOpaque::Uniform* data = reinterpret_cast<render::material::ShadelessColoredMatteNonreflectiveShadowlessOpaque::Uniform*>(u->get_data());
    ShadelessColoredMatteNonreflectiveShadowlessOpaque* shd = reinterpret_cast<ShadelessColoredMatteNonreflectiveShadowlessOpaque*>(pip->get_shader());
    shd->use();
    shd->set_mvp(data->mvp.data());
    shd->set_color(data->color.data());
}

gearoenix::gles3::shader::ShadelessColoredMatteNonreflectiveShadowlessOpaque::ShadelessColoredMatteNonreflectiveShadowlessOpaque(core::Id my_id, Engine* eng, core::sync::EndCaller<core::sync::EndCallerIgnore> end)
    : Shader(my_id, eng, end)
{
    eng->add_load_function([this, end] {
        create_program();
        const std::string pvs = "#version 330\n"
                                "layout(location = 0) in vec3 vertex;\n"
                                "uniform mat4 mvp;\n"
                                "void main()\n"
                                "{\n"
                                "    gl_Position = mvp * vec4(vertex, 1.0);\n"
                                "}\n";
        const std::string pfs = "#version 330\n"
                                "uniform vec4 color;\n"
                                "out vec4 frag_color;\n"
                                "void main()\n"
                                "{\n"
                                "    frag_color = color;\n"
                                "}\n";
        vtx_shd = add_shader_to_program(pvs, GL_VERTEX_SHADER);
        frg_shd = add_shader_to_program(pfs, GL_FRAGMENT_SHADER);
        run();
        vtx_att_ind = glGetAttribLocation(shader_program, "vertex");
        mvp = get_uniform_location("mvp");
        color = get_uniform_location("color");
        (void)end;
    });
}

gearoenix::gles3::shader::ShadelessColoredMatteNonreflectiveShadowlessOpaque::~ShadelessColoredMatteNonreflectiveShadowlessOpaque()
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

void gearoenix::gles3::shader::ShadelessColoredMatteNonreflectiveShadowlessOpaque::use()
{
    glUseProgram(shader_program);
}

const std::vector<gearoenix::render::shader::stage::Id>& gearoenix::gles3::shader::ShadelessColoredMatteNonreflectiveShadowlessOpaque::get_stages_ids() const
{
    return graphic_2_stage;
}

void gearoenix::gles3::shader::ShadelessColoredMatteNonreflectiveShadowlessOpaque::set_mvp(const GLfloat* data)
{
    glUniformMatrix4fv(mvp, 1, GL_FALSE, data);
}

void gearoenix::gles3::shader::ShadelessColoredMatteNonreflectiveShadowlessOpaque::set_color(const GLfloat* data)
{
    glUniform4fv(color, 1, data);
}
#endif
