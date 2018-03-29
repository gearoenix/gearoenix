#include "gles3-shd-directional-d2-speculated-nonreflective-shadowless-opaque.hpp"
#ifdef USE_OPENGL_ES3
#include "../../system/sys-log.hpp"
#include "../buffer/gles3-buf-uniform.hpp"
#include "../gles3-engine.hpp"
#include "../pipeline/gles3-pip-pipeline.hpp"
#include "../texture/gles3-txt-2d.hpp"

gearoenix::gles3::shader::DirectionalD2SpeculatedNonreflectiveShadowlessOpaque::Resources::Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u)
    : render::material::DirectionalD2SpeculatedNonreflectiveShadowlessOpaque::Resources(e, pip, u)
{
}

void gearoenix::gles3::shader::DirectionalD2SpeculatedNonreflectiveShadowlessOpaque::Resources::bind()
{
    render::material::DirectionalD2SpeculatedNonreflectiveShadowlessOpaque::Uniform* data = reinterpret_cast<render::material::DirectionalD2SpeculatedNonreflectiveShadowlessOpaque::Uniform*>(u->get_data());
    DirectionalD2SpeculatedNonreflectiveShadowlessOpaque* shd = reinterpret_cast<DirectionalD2SpeculatedNonreflectiveShadowlessOpaque*>(pip->get_shader());
    shd->use();

    shd->set_ambl_color(data->ambl_color.data());
    shd->set_eye(data->eye.data());
    shd->set_m(data->m.data());
    shd->set_vp(data->vp.data());
    shd->set_spec_color(data->spec_color.data());
    shd->set_spec_factors(data->spec_factors.data());
    shd->set_sun(data->sun.data());
    shd->set_sun_color(data->sun_color.data());
    reinterpret_cast<texture::Texture2D*>(txt)->bind(GL_TEXTURE0);
}

gearoenix::gles3::shader::DirectionalD2SpeculatedNonreflectiveShadowlessOpaque::DirectionalD2SpeculatedNonreflectiveShadowlessOpaque(core::Id my_id, Engine* eng, core::sync::EndCaller<core::sync::EndCallerIgnore> end)
    : Shader(my_id, eng, end)
{
    eng->add_load_function([this, end] {
        create_program();
        const std::string pvs = "#version 330\n"
                                "layout(location = 0) in vec3 vertex;\n"
                                "layout(location = 1) in vec3 normal;\n"
                                "layout(location = 2) in vec2 uv;\n"
                                "out vec2 out_uv;\n"
                                "out float out_diffuse;\n"
                                "out float out_speculare;\n"
                                "uniform mat4 vp;\n"
                                "uniform mat4 m;\n"
                                "uniform vec3 sun;\n"
                                "uniform vec3 eye;\n"
                                "void main()\n"
                                "{\n"
                                "    vec4 position = vec4(vertex, 1.0);\n"
                                "    vec4 world_position = m * position;\n"
                                "    vec3 world_normal = normalize((m * vec4(normal, 0.0)).xyz);\n"
                                "    vec3 reflected = reflect(sun, world_normal);\n"
                                "    out_uv = uv;\n"
                                "    out_diffuse = dot(sun, world_normal);\n"
                                "    out_speculare = -dot(normalize(eye - world_position.xyz), reflected);\n"
                                "    gl_Position = vp * world_position;\n"
                                "}\n";
        const std::string pfs = "#version 330\n"
                                "in vec2 out_uv;\n"
                                "in float out_diffuse;\n"
                                "in float out_speculare;\n"
                                "out vec4 frag_color;\n"
                                "uniform vec3 sun_color;\n"
                                "uniform vec3 spec_color;\n"
                                "uniform vec3 spec_factors;\n"
                                "uniform vec3 ambl_color;\n"
                                "uniform sampler2D txt;\n"
                                "void main()\n"
                                "{\n"
                                "    vec3 txt_color = texture(txt, out_uv).xyz;\n"
                                "    vec3 ambl_light = txt_color * ambl_color;\n"
                                "    float diff_fac = smoothstep(0.2, 0.5, out_diffuse);\n"
                                "    float spec_fac = smoothstep(spec_factors[0], spec_factors[1], out_speculare) * spec_factors[2];\n"
                                "    vec3 diff_color = txt_color * diff_fac * sun_color;\n"
                                "    vec3 spc_color = spec_color * spec_fac;\n"
                                "    frag_color = vec4(diff_color + ambl_light + spc_color, 1.0);\n"
                                "}\n";
        vtx_shd = add_shader_to_program(pvs, GL_VERTEX_SHADER);
        frg_shd = add_shader_to_program(pfs, GL_FRAGMENT_SHADER);
        run();
        vtx_att_ind = glGetAttribLocation(shader_program, "vertex");
        nrm_att_ind = glGetAttribLocation(shader_program, "normal");
        uv_att_ind = glGetAttribLocation(shader_program, "uv");
        vp = get_uniform_location("vp");
        m = get_uniform_location("m");
        sun = get_uniform_location("sun");
        sun_color = get_uniform_location("sun_color");
        eye = get_uniform_location("eye");
        spec_color = get_uniform_location("spec_color");
        spec_factors = get_uniform_location("spec_factors");
        ambl_color = get_uniform_location("ambl_color");
        txt = get_uniform_location("txt");
        (void)end;
    });
}

gearoenix::gles3::shader::DirectionalD2SpeculatedNonreflectiveShadowlessOpaque::~DirectionalD2SpeculatedNonreflectiveShadowlessOpaque()
{
    eng->add_load_function([this] {
        end_object(vtx_shd);
        end_object(frg_shd);
        end_program();
    });
}

void gearoenix::gles3::shader::DirectionalD2SpeculatedNonreflectiveShadowlessOpaque::use()
{
    glUseProgram(shader_program);
    glUniform1i(txt, 0);
}

const std::vector<gearoenix::render::shader::stage::Id>& gearoenix::gles3::shader::DirectionalD2SpeculatedNonreflectiveShadowlessOpaque::get_stages_ids() const
{
    return graphic_2_stage;
}

void gearoenix::gles3::shader::DirectionalD2SpeculatedNonreflectiveShadowlessOpaque::set_vp(const GLfloat* data)
{
    glUniformMatrix4fv(vp, 1, GL_FALSE, data);
}

void gearoenix::gles3::shader::DirectionalD2SpeculatedNonreflectiveShadowlessOpaque::set_m(const GLfloat* data)
{
    glUniformMatrix4fv(m, 1, GL_FALSE, data);
}

void gearoenix::gles3::shader::DirectionalD2SpeculatedNonreflectiveShadowlessOpaque::set_sun(const GLfloat* data)
{
    glUniform3fv(sun, 1, data);
}

void gearoenix::gles3::shader::DirectionalD2SpeculatedNonreflectiveShadowlessOpaque::set_sun_color(const GLfloat* data)
{
    glUniform3fv(sun_color, 1, data);
}

void gearoenix::gles3::shader::DirectionalD2SpeculatedNonreflectiveShadowlessOpaque::set_eye(const GLfloat* data)
{
    glUniform3fv(eye, 1, data);
}

void gearoenix::gles3::shader::DirectionalD2SpeculatedNonreflectiveShadowlessOpaque::set_spec_color(const GLfloat* data)
{
    glUniform3fv(spec_color, 1, data);
}

void gearoenix::gles3::shader::DirectionalD2SpeculatedNonreflectiveShadowlessOpaque::set_spec_factors(const GLfloat* data)
{
    glUniform3fv(spec_factors, 1, data);
}

void gearoenix::gles3::shader::DirectionalD2SpeculatedNonreflectiveShadowlessOpaque::set_ambl_color(const GLfloat* data)
{
    glUniform3fv(ambl_color, 1, data);
}
#endif
