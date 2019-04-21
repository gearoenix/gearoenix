#include "gles3-shd-directional-colored-matte-nonreflective-shadowless-opaque.hpp"
#ifdef USE_OPENGL_ES3
#include "../../system/sys-log.hpp"
#include "../buffer/gles3-buf-uniform.hpp"
#include "../gles3-engine.hpp"
#include "../pipeline/gles3-pip-pipeline.hpp"

gearoenix::gles3::shader::DirectionalColoredMatteNonreflectiveShadowlessOpaque::Resources::Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u)
    : render::material::DirectionalColoredMatteNonreflectiveShadowlessOpaque::Resources(e, pip, u)
{
}

void gearoenix::gles3::shader::DirectionalColoredMatteNonreflectiveShadowlessOpaque::Resources::bind()
{
    render::material::DirectionalColoredMatteNonreflectiveShadowlessOpaque::Uniform* data = static_cast<render::material::DirectionalColoredMatteNonreflectiveShadowlessOpaque::Uniform*>(u->get_data());
    DirectionalColoredMatteNonreflectiveShadowlessOpaque* shd = static_cast<DirectionalColoredMatteNonreflectiveShadowlessOpaque*>(pip->get_shader());
    shd->use();
    shd->set_ambl_color(data->ambl_color.data());
    shd->set_m(data->m.data());
    shd->set_mvp(data->mvp.data());
    shd->set_sun(data->sun.data());
    shd->set_sun_color(data->sun_color.data());
}

gearoenix::gles3::shader::DirectionalColoredMatteNonreflectiveShadowlessOpaque::DirectionalColoredMatteNonreflectiveShadowlessOpaque(core::Id my_id, Engine* eng, core::sync::EndCaller<core::sync::EndCallerIgnore> end)
    : Shader(my_id, eng, end)
{
    eng->add_load_function([this, end] {
        create_program();
        std::string pvs = "precision highp sampler2D;\n"
                          "precision highp float;\n"
                          "attribute vec3 vertex;\n"
                          "attribute vec3 normal;\n"
                          "varying float out_diffuse;\n"
                          "uniform mat4 mvp;\n"
                          "uniform mat4 m;\n"
                          "uniform vec3 sun;\n"
                          "void main()\n"
                          "{\n"
                          "    vec3 world_normal = normalize((m * vec4(normal, 0.0)).xyz);\n"
                          "    out_diffuse = dot(sun, world_normal);\n"
                          "    gl_Position = mvp * vec4(vertex, 1.0);\n"
                          "}\n";
        std::string pfs = "precision highp sampler2D;\n"
                          "precision highp float;\n"
                          "varying float out_diffuse;\n"
                          "uniform vec3 sun_color;\n"
                          "uniform vec3 ambl_color;\n"
                          "void main()\n"
                          "{\n"
                          "    float diff = smoothstep(0.2, 0.5, out_diffuse);\n"
                          "    vec3 final_color = sun_color * diff;\n"
                          "    final_color += ambl_color;\n"
                          "    gl_FragColor = vec4(final_color, 1.0);\n"
                          "}\n";
        vtx_shd = add_shader_to_program(pvs, GL_VERTEX_SHADER);
        frg_shd = add_shader_to_program(pfs, GL_FRAGMENT_SHADER);
        run();
        vtx_att_ind = glGetAttribLocation(shader_program, "vertex");
        nrm_att_ind = glGetAttribLocation(shader_program, "normal");
        m = get_uniform_location("m");
        mvp = get_uniform_location("mvp");
        sun = get_uniform_location("sun");
        sun_color = get_uniform_location("sun_color");
        ambl_color = get_uniform_location("ambl_color");
        (void)end;
    });
}

gearoenix::gles3::shader::DirectionalColoredMatteNonreflectiveShadowlessOpaque::~DirectionalColoredMatteNonreflectiveShadowlessOpaque()
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

void gearoenix::gles3::shader::DirectionalColoredMatteNonreflectiveShadowlessOpaque::use()
{
    glUseProgram(shader_program);
}

const std::vector<gearoenix::render::shader::stage::Id>& gearoenix::gles3::shader::DirectionalColoredMatteNonreflectiveShadowlessOpaque::get_stages_ids() const
{
    return graphic_2_stage;
}

void gearoenix::gles3::shader::DirectionalColoredMatteNonreflectiveShadowlessOpaque::set_mvp(const GLfloat* data)
{
    glUniformMatrix4fv(mvp, 1, GL_FALSE, data);
}

void gearoenix::gles3::shader::DirectionalColoredMatteNonreflectiveShadowlessOpaque::set_m(const GLfloat* data)
{
    glUniformMatrix4fv(m, 1, GL_FALSE, data);
}

void gearoenix::gles3::shader::DirectionalColoredMatteNonreflectiveShadowlessOpaque::set_sun(const GLfloat* data)
{
    glUniform3fv(sun, 1, data);
}

void gearoenix::gles3::shader::DirectionalColoredMatteNonreflectiveShadowlessOpaque::set_sun_color(const GLfloat* data)
{
    glUniform3fv(sun_color, 1, data);
}

void gearoenix::gles3::shader::DirectionalColoredMatteNonreflectiveShadowlessOpaque::set_ambl_color(const GLfloat* data)
{
    glUniform3fv(ambl_color, 1, data);
}

#endif
