#include "gles2-shd-directional-textured-speculated-nocube-noshadow-opaque.hpp"
#ifdef USE_OPENGL_ES2
#include "../../system/sys-log.hpp"
#include "../buffer/gles2-buf-uniform.hpp"
#include "../gles2-engine.hpp"
#include "../pipeline/gles2-pip-pipeline.hpp"
#include "../texture/gles2-txt-2d.hpp"

gearoenix::gles2::shader::DirectionalTexturedSpeculatedNocubeNoshadowOpaque::Resources::Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u)
    : render::material::DirectionalTexturedSpeculatedNocubeFullshadowOpaque::Resources(e, pip, u)
{
}

void gearoenix::gles2::shader::DirectionalTexturedSpeculatedNocubeNoshadowOpaque::Resources::bind()
{
    render::material::DirectionalTexturedSpeculatedNocubeFullshadowOpaque::Uniform* data = reinterpret_cast<render::material::DirectionalTexturedSpeculatedNocubeFullshadowOpaque::Uniform*>(u->get_data());
    DirectionalTexturedSpeculatedNocubeNoshadowOpaque* shd = reinterpret_cast<DirectionalTexturedSpeculatedNocubeNoshadowOpaque*>(pip->get_shader());
    shd->use();
    //    shd->set_ambl_color(data->ambl_color.data());
    shd->set_ambl_color(math::Vec3(1.0f).data());
    //    shd->set_eye(data->eye_color.data());
    shd->set_eye(math::Vec3(1.0f).data());
    //    shd->set_m(data->m.data());
    shd->set_m(math::Mat4x4().data());
    //        shd->set_mvp(data->mvp.data());
    shd->set_mvp(math::Mat4x4().data());
    //    shd->set_spec_color(data->spec_color.data());
    shd->set_spec_color(math::Vec3(1.0f).data());
    //    shd->set_spec_factor(data->spec_factor);
    shd->set_spec_factor(1.0f);
    //    shd->set_sun(data->sun.data());
    shd->set_sun(math::Vec3(1.0f).data());
    //    shd->set_sun_color(data->sun_color.data());
    shd->set_sun_color(math::Vec3(1.0f).data());
    reinterpret_cast<texture::Texture2D*>(txt)->bind(GL_TEXTURE0);
}

gearoenix::gles2::shader::DirectionalTexturedSpeculatedNocubeNoshadowOpaque::DirectionalTexturedSpeculatedNocubeNoshadowOpaque(Engine* eng, std::shared_ptr<core::EndCaller> end)
    : Shader(eng, end)
{
    eng->add_load_function([this, end] {
        create_program();
        const std::string pvs = "precision highp sampler2D;\n"
                                "precision highp float;\n"
                                "attribute vec3 vertex;\n"
                                "attribute vec3 normal;\n"
                                "attribute vec2 uv;\n"
                                "varying vec2 out_uv;\n"
                                "varying vec3 out_normal;\n"
                                "varying vec3 out_pos;\n"
                                "uniform mat4 mvp;\n"
                                "uniform mat4 m;\n"
                                "void main()\n"
                                "{\n"
                                "    out_uv = uv;\n"
                                "    out_normal = normalize((m * vec4(normal, 0.0)).xyz);\n"
                                "    vec4 pos = vec4(vertex, 1.0);\n"
                                "    out_pos = (m * pos).xyz;\n"
                                "    gl_Position = mvp * pos;\n"
                                "}\n";
        const std::string pfs = "precision highp sampler2D;\n"
                                "precision highp float;\n"
                                "varying vec2 out_uv;\n"
                                "varying vec3 out_normal;\n"
                                "varying vec3 out_pos;\n"
                                "uniform vec3 sun;\n"
                                "uniform vec3 sun_color;\n"
                                "uniform vec3 eye;\n"
                                "uniform vec3 spec_color;\n"
                                "uniform float spec_factor;\n"
                                "uniform vec3 ambl_color;\n"
                                "uniform sampler2D txt;\n"
                                "void main()\n"
                                "{\n"
                                "    float diffuse = -dot(sun, out_normal);\n"
                                "    vec3 reflected = reflect(sun, out_normal);\n"
                                "    float speculare = dot(normalize(eye - out_pos), reflected);\n"
                                "    float diff_fac = smoothstep(0.2, 0.4, diffuse) * 0.5;\n"
                                "    float spec_fac = smoothstep(0.7, 0.9, speculare) * spec_factor;\n"
                                "    vec3 txt_color = texture2D(txt, out_uv).xyz;\n"
                                "    vec3 ambl_light = txt_color * ambl_color;\n"
                                "    vec3 diff_color = txt_color * diff_fac * sun_color;\n"
                                "    vec3 spc_color = spec_color * spec_fac;\n"
                                "    gl_FragColor = vec4(diff_color + ambl_light + spc_color, 1.0);\n"
                                "}\n";
        vtx_shd = add_shader_to_program(pvs, GL_VERTEX_SHADER);
        frg_shd = add_shader_to_program(pfs, GL_FRAGMENT_SHADER);
        run();
        vtx_att_ind = glGetAttribLocation(shader_program, "vertex");
        nrm_att_ind = glGetAttribLocation(shader_program, "normal");
        uv_att_ind = glGetAttribLocation(shader_program, "uv");
        mvp = get_uniform_location("mvp");
        m = get_uniform_location("m");
        sun = get_uniform_location("sun");
        sun_color = get_uniform_location("sun_color");
        eye = get_uniform_location("eye");
        spec_color = get_uniform_location("spec_color");
        spec_factor = get_uniform_location("spec_factor");
        ambl_color = get_uniform_location("ambl_color");
        txt = get_uniform_location("txt");
        (void)end;
    });
}

gearoenix::gles2::shader::DirectionalTexturedSpeculatedNocubeNoshadowOpaque::~DirectionalTexturedSpeculatedNocubeNoshadowOpaque()
{
    eng->add_load_function([this] {
        end_object(vtx_shd);
        end_object(frg_shd);
        end_program();
    });
}

void gearoenix::gles2::shader::DirectionalTexturedSpeculatedNocubeNoshadowOpaque::use()
{
    glUseProgram(shader_program);
    glEnableVertexAttribArray(vtx_att_ind);
    glEnableVertexAttribArray(nrm_att_ind);
    glEnableVertexAttribArray(uv_att_ind);
    ////////////////////////////////////////////////////////////
    glVertexAttribPointer(vtx_att_ind, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
    glVertexAttribPointer(nrm_att_ind, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glVertexAttribPointer(uv_att_ind, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
    ////////////////////////////////////////
    glUniform1i(txt, 0);
    //Tempppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp
    const GLfloat data1[] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    };
    const GLfloat data2[] = {
        1.0f, 1.0f, 1.0f,
    };
    const GLfloat data3[] = {
        0.3f, 0.3f, 0.3f,
    };
    glUniformMatrix4fv(mvp, 1, GL_FALSE, data1);
    glUniformMatrix4fv(m, 1, GL_FALSE, data1);
    glUniform3fv(sun, 1, data1);
    glUniform3fv(sun_color, 1, data2);
    glUniform3fv(eye, 1, data1);
    glUniform3fv(spec_color, 1, data2);
    glUniform1f(spec_factor, 0.2f);
    glUniform3fv(ambl_color, 1, data3);
}

const std::vector<gearoenix::render::shader::stage::Id>& gearoenix::gles2::shader::DirectionalTexturedSpeculatedNocubeNoshadowOpaque::get_stages_ids() const
{
    return graphic_2_stage;
}

void gearoenix::gles2::shader::DirectionalTexturedSpeculatedNocubeNoshadowOpaque::set_mvp(const GLfloat* data)
{
    glUniformMatrix4fv(mvp, 1, GL_FALSE, data);
}

void gearoenix::gles2::shader::DirectionalTexturedSpeculatedNocubeNoshadowOpaque::set_m(const GLfloat* data)
{
    glUniformMatrix4fv(m, 1, GL_FALSE, data);
}

void gearoenix::gles2::shader::DirectionalTexturedSpeculatedNocubeNoshadowOpaque::set_sun(const GLfloat* data)
{
    glUniform3fv(sun, 1, data);
}

void gearoenix::gles2::shader::DirectionalTexturedSpeculatedNocubeNoshadowOpaque::set_sun_color(const GLfloat* data)
{
    glUniform3fv(sun_color, 1, data);
}

void gearoenix::gles2::shader::DirectionalTexturedSpeculatedNocubeNoshadowOpaque::set_eye(const GLfloat* data)
{
    glUniform3fv(eye, 1, data);
}

void gearoenix::gles2::shader::DirectionalTexturedSpeculatedNocubeNoshadowOpaque::set_spec_color(const GLfloat* data)
{
    glUniform3fv(spec_color, 1, data);
}

void gearoenix::gles2::shader::DirectionalTexturedSpeculatedNocubeNoshadowOpaque::set_spec_factor(GLfloat data)
{
    glUniform1f(spec_factor, data);
}

void gearoenix::gles2::shader::DirectionalTexturedSpeculatedNocubeNoshadowOpaque::set_ambl_color(const GLfloat* data)
{
    glUniform3fv(ambl_color, 1, data);
}
#endif
