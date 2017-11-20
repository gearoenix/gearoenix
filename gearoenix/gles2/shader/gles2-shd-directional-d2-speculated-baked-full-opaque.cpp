#include "gles2-shd-directional-d2-speculated-baked-full-opaque.hpp"
#ifdef USE_OPENGL_ES2
#include "../../system/sys-log.hpp"
#include "../buffer/gles2-buf-uniform.hpp"
#include "../gles2-engine.hpp"
#include "../pipeline/gles2-pip-pipeline.hpp"
#include "../texture/gles2-txt-2d.hpp"
#include "../texture/gles2-txt-cube.hpp"

gearoenix::gles2::shader::DirectionalD2SpeculatedBakedFullOpaque::Resources::Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u)
    : render::material::DirectionalD2SpeculatedBakedFullOpaque::Resources(e, pip, u)
{
}

void gearoenix::gles2::shader::DirectionalD2SpeculatedBakedFullOpaque::Resources::bind()
{
    render::material::DirectionalD2SpeculatedBakedFullOpaque::Uniform* data = reinterpret_cast<render::material::DirectionalD2SpeculatedBakedFullOpaque::Uniform*>(u->get_data());
    DirectionalD2SpeculatedBakedFullOpaque* shd = reinterpret_cast<DirectionalD2SpeculatedBakedFullOpaque*>(pip->get_shader());
    shd->use();

    shd->set_ambl_color(data->ambl_color.data());
    shd->set_db(data->db.data());
    shd->set_eye(data->eye.data());
    shd->set_m(data->m.data());
    shd->set_rfl_fac(data->rfl_fac);
    shd->set_spec_color(data->spec_color.data());
    shd->set_spec_factors(data->spec_factors.data());
    shd->set_sun(data->sun.data());
    shd->set_sun_color(data->sun_color.data());
    shd->set_vp(data->vp.data());

    reinterpret_cast<texture::Cube*>(env)->bind(GL_TEXTURE0);
    reinterpret_cast<texture::Texture2D*>(txt)->bind(GL_TEXTURE1);
    reinterpret_cast<texture::Texture2D*>(shdtxt)->bind(GL_TEXTURE2);
}

gearoenix::gles2::shader::DirectionalD2SpeculatedBakedFullOpaque::DirectionalD2SpeculatedBakedFullOpaque(Engine* eng, std::shared_ptr<core::EndCaller> end)
    : Shader(eng, end)
{
    eng->add_load_function([this, end] {
        create_program();
        const std::string pvs = "precision highp sampler2D;\n"
                                "precision highp samplerCube;\n"
                                "precision highp float;\n"
                                "attribute vec3 vertex;\n"
                                "attribute vec3 normal;\n"
                                "attribute vec2 uv;\n"
                                "varying vec2 out_uv;\n"
                                "varying vec3 out_shd;\n"
                                "varying float out_diffuse;\n"
                                "varying float out_speculare;\n"
                                "varying vec3 out_eye_reflected;\n"
                                "varying float out_bias;\n"
                                "uniform mat4 vp;\n"
                                "uniform mat4 m;\n"
                                "uniform mat4 db;\n"
                                "uniform vec3 sun;\n"
                                "uniform vec3 eye;\n"
                                "void main()\n"
                                "{\n"
                                "    out_uv = uv;\n"
                                "    vec4 position = vec4(vertex, 1.0);\n"
                                "    vec4 world_position = m * position;\n"
                                "    vec3 world_normal = normalize((m * vec4(normal, 0.0)).xyz);\n"
                                "    vec3 eye_direction = normalize(world_position.xyz - eye);\n"
                                "    vec3 reflected = reflect(sun, world_normal);\n"
                                "    out_eye_reflected = reflect(eye_direction, world_normal);\n"
                                "    out_diffuse = dot(sun, world_normal);\n"
                                "    out_speculare = dot(eye_direction, reflected);\n"
                                "    out_shd = (db * world_position).xyz;\n"
                                "    out_bias = clamp(0.005 * tan(acos(abs(out_diffuse))), 0.0, 0.01);\n"
                                "    gl_Position = vp * world_position;\n"
                                "}\n";
        const std::string pfs = "precision highp sampler2D;\n"
                                "precision highp samplerCube;\n"
                                "precision highp float;\n"
                                "varying vec2 out_uv;\n"
                                "varying vec3 out_shd;\n"
                                "varying float out_diffuse;\n"
                                "varying float out_speculare;\n"
                                "varying vec3 out_eye_reflected;\n"
                                "varying float out_bias;\n"
                                "uniform vec3 sun_color;\n"
                                "uniform vec3 spec_color;\n"
                                "uniform vec3 spec_factors;\n"
                                "uniform vec3 ambl_color;\n"
                                "uniform float rfl_fac;\n"
                                "uniform samplerCube rfl_env;\n"
                                "uniform sampler2D txt;\n"
                                "uniform sampler2D shdtxt;\n"
                                "void main()\n"
                                "{\n"
                                "    vec3 txt_color = texture2D(txt, out_uv).xyz;\n"
                                "    vec3 ambl_light = txt_color * ambl_color;\n"
                                "    vec3 env_color = textureCube(rfl_env, out_eye_reflected).xyz;\n"
                                "    float diff_fac = smoothstep(0.2, 0.5, out_diffuse);\n"
                                "    if(diff_fac < 0.001)\n"
                                "    {\n"
                                "        gl_FragColor = vec4(ambl_light + (env_color * rfl_fac), 1.0);\n"
                                "    }\n"
                                "    else\n"
                                "    {\n"
                                "        vec4 v = texture2D(shdtxt, out_shd.xy);\n"
                                "        float d = v.y;\n"
                                "        d /= 256.0;\n"
                                "        d += v.x;\n"
                                "        if(d < out_shd.z - out_bias)\n"
                                "        {\n"
                                "            gl_FragColor = vec4(ambl_light + (env_color * rfl_fac), 1.0);\n"
                                "        }\n"
                                "        else\n"
                                "        {\n"
                                "            float spec_fac = smoothstep(spec_factors[0], spec_factors[1], out_speculare) * spec_factors[2];\n"
                                "            vec3 final_color = diff_fac * txt_color * sun_color;\n"
                                "            final_color = mix(final_color, env_color, rfl_fac);\n"
                                "            final_color += spec_color * spec_fac;\n"
                                "            final_color += ambl_light;\n"
                                "            gl_FragColor = vec4(final_color, 1.0);\n"
                                "        }\n"
                                "    }\n"
                                "}\n";
        vtx_shd = add_shader_to_program(pvs, GL_VERTEX_SHADER);
        frg_shd = add_shader_to_program(pfs, GL_FRAGMENT_SHADER);

        link();

        glUseProgram(shader_program);

        vtx_att_ind = glGetAttribLocation(shader_program, "vertex");
        nrm_att_ind = glGetAttribLocation(shader_program, "normal");
        uv_att_ind = glGetAttribLocation(shader_program, "uv");

        ambl_color = get_uniform_location("ambl_color");
        db = get_uniform_location("db");
        eye = get_uniform_location("eye");
        m = get_uniform_location("m");
        rfl_fac = get_uniform_location("rfl_fac");
        spec_color = get_uniform_location("spec_color");
        spec_factors = get_uniform_location("spec_factors");
        sun = get_uniform_location("sun");
        sun_color = get_uniform_location("sun_color");
        vp = get_uniform_location("vp");

        rfl_env = get_uniform_location("rfl_env");
        txt = get_uniform_location("txt");
        shdtxt = get_uniform_location("shdtxt");

        glUniform1i(rfl_env, 0);
        glUniform1i(txt, 1);
        glUniform1i(shdtxt, 2);

        validate();
        (void)end;
    });
}

gearoenix::gles2::shader::DirectionalD2SpeculatedBakedFullOpaque::~DirectionalD2SpeculatedBakedFullOpaque()
{
    eng->add_load_function([this] {
        end_object(vtx_shd);
        end_object(frg_shd);
        end_program();
    });
}

void gearoenix::gles2::shader::DirectionalD2SpeculatedBakedFullOpaque::use()
{
    glUseProgram(shader_program);
    glEnableVertexAttribArray(vtx_att_ind);
    glEnableVertexAttribArray(nrm_att_ind);
    glEnableVertexAttribArray(uv_att_ind);
    ////////////////////////////////////////////////////////////
    glVertexAttribPointer(vtx_att_ind, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
    glVertexAttribPointer(nrm_att_ind, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glVertexAttribPointer(uv_att_ind, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
    glUniform1i(rfl_env, 0);
    glUniform1i(txt, 1);
    glUniform1i(shdtxt, 2);
}

const std::vector<gearoenix::render::shader::stage::Id>& gearoenix::gles2::shader::DirectionalD2SpeculatedBakedFullOpaque::get_stages_ids() const
{
    return graphic_2_stage;
}

void gearoenix::gles2::shader::DirectionalD2SpeculatedBakedFullOpaque::set_vp(const GLfloat* data)
{
    glUniformMatrix4fv(vp, 1, GL_FALSE, data);
}

void gearoenix::gles2::shader::DirectionalD2SpeculatedBakedFullOpaque::set_m(const GLfloat* data)
{
    glUniformMatrix4fv(m, 1, GL_FALSE, data);
}

void gearoenix::gles2::shader::DirectionalD2SpeculatedBakedFullOpaque::set_sun(const GLfloat* data)
{
    glUniform3fv(sun, 1, data);
}

void gearoenix::gles2::shader::DirectionalD2SpeculatedBakedFullOpaque::set_sun_color(const GLfloat* data)
{
    glUniform3fv(sun_color, 1, data);
}

void gearoenix::gles2::shader::DirectionalD2SpeculatedBakedFullOpaque::set_eye(const GLfloat* data)
{
    glUniform3fv(eye, 1, data);
}

void gearoenix::gles2::shader::DirectionalD2SpeculatedBakedFullOpaque::set_spec_color(const GLfloat* data)
{
    glUniform3fv(spec_color, 1, data);
}

void gearoenix::gles2::shader::DirectionalD2SpeculatedBakedFullOpaque::set_spec_factors(const GLfloat* data)
{
    glUniform3fv(spec_factors, 1, data);
}

void gearoenix::gles2::shader::DirectionalD2SpeculatedBakedFullOpaque::set_ambl_color(const GLfloat* data)
{
    glUniform3fv(ambl_color, 1, data);
}

void gearoenix::gles2::shader::DirectionalD2SpeculatedBakedFullOpaque::set_db(const GLfloat* data)
{
    glUniformMatrix4fv(db, 1, GL_FALSE, data);
}

void gearoenix::gles2::shader::DirectionalD2SpeculatedBakedFullOpaque::set_rfl_fac(const GLfloat data)
{
    glUniform1f(rfl_fac, data);
}
#endif
