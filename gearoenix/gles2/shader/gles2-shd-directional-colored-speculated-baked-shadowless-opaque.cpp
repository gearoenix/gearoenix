#include "gles2-shd-directional-colored-speculated-baked-shadowless-opaque.hpp"
#ifdef USE_OPENGL_ES2
#include "../../system/sys-log.hpp"
#include "../buffer/gles2-buf-uniform.hpp"
#include "../gles2-engine.hpp"
#include "../pipeline/gles2-pip-pipeline.hpp"
#include "../texture/gles2-txt-cube.hpp"

gearoenix::gles2::shader::DirectionalColoredSpeculatedBakedShadowlessOpaque::Resources::Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u)
    : render::material::DirectionalColoredSpeculatedBakedShadowlessOpaque::Resources(e, pip, u)
{
}

void gearoenix::gles2::shader::DirectionalColoredSpeculatedBakedShadowlessOpaque::Resources::bind()
{
    render::material::DirectionalColoredSpeculatedBakedShadowlessOpaque::Uniform* data = reinterpret_cast<render::material::DirectionalColoredSpeculatedBakedShadowlessOpaque::Uniform*>(u->get_data());
    DirectionalColoredSpeculatedBakedShadowlessOpaque* shd = reinterpret_cast<DirectionalColoredSpeculatedBakedShadowlessOpaque*>(pip->get_shader());
    shd->use();

    shd->set_ambl_color(data->ambl_color.data());
    shd->set_eye(data->eye.data());
    shd->set_m(data->m.data());
    shd->set_rfl_fac(data->rfl_fac);
    shd->set_spec_color(data->spec_color.data());
    shd->set_spec_factors(data->spec_factors.data());
    shd->set_sun(data->sun.data());
    shd->set_sun_color(data->sun_color.data());
    shd->set_vp(data->vp.data());

    reinterpret_cast<texture::Cube*>(env)->bind(GL_TEXTURE0);
}

gearoenix::gles2::shader::DirectionalColoredSpeculatedBakedShadowlessOpaque::DirectionalColoredSpeculatedBakedShadowlessOpaque(core::Id my_id, Engine* eng, core::sync::EndCaller<core::sync::EndCallerIgnore> end)
    : Shader(my_id, eng, end)
{
    eng->add_load_function([this, end] {
        CHECK_FOR_GRAPHIC_API_ERROR;
        create_program();
        const std::string pvs = "precision highp samplerCube;\n"
                                "precision highp float;\n"
                                "attribute vec3 vertex;\n"
                                "attribute vec3 normal;\n"
                                "varying float out_diffuse;\n"
                                "varying float out_speculare;\n"
                                "varying vec3 out_eye_reflected;\n"
                                "uniform mat4 vp;\n"
                                "uniform mat4 m;\n"
                                "uniform vec3 eye;\n"
                                "uniform vec3 sun;\n"
                                "void main()\n"
                                "{\n"
                                "    vec4 world_position = m * vec4(vertex, 1.0);\n"
                                "    vec3 world_normal = normalize((m * vec4(normal, 0.0)).xyz);\n"
                                "    vec3 eye_direction = normalize(world_position.xyz - eye);\n"
                                "    out_eye_reflected = reflect(eye_direction, world_normal);\n"
                                "    vec3 reflected = reflect(sun, world_normal);\n"
                                "    out_diffuse = dot(sun, world_normal);\n"
                                "    out_speculare = dot(eye_direction, reflected);\n"
                                "    gl_Position = vp * world_position;\n"
                                "}\n";
        const std::string pfs = "precision highp samplerCube;\n"
                                "precision highp float;\n"
                                "varying float out_diffuse;\n"
                                "varying float out_speculare;\n"
                                "varying vec3 out_eye_reflected;\n"
                                "uniform vec3 sun_color;\n"
                                "uniform vec3 spec_color;\n"
                                "uniform vec3 spec_factors;\n"
                                "uniform vec3 ambl_color;\n"
                                "uniform float rfl_fac;\n"
                                "uniform samplerCube rfl_env;\n"
                                "void main()\n"
                                "{\n"
                                "    vec3 env_color = textureCube(rfl_env, out_eye_reflected).xyz;\n"
                                "    float spec = smoothstep(spec_factors[0], spec_factors[1], out_speculare) * spec_factors[2];\n"
                                "    float diff = smoothstep(0.2, 0.5, out_diffuse);\n"
                                "    vec3 final_color = diff * sun_color;\n"
                                "    final_color = mix(final_color, env_color, rfl_fac);\n"
                                "    final_color += spec * spec_color;\n"
                                "    final_color += ambl_color;\n"
                                "    gl_FragColor = vec4(final_color, 1.0);\n"
                                "}\n";
        vtx_shd = add_shader_to_program(pvs, GL_VERTEX_SHADER);
        frg_shd = add_shader_to_program(pfs, GL_FRAGMENT_SHADER);

        run();

        vtx_att_ind = glGetAttribLocation(shader_program, "vertex");
        nrm_att_ind = glGetAttribLocation(shader_program, "normal");

        ambl_color = get_uniform_location("ambl_color");
        eye = get_uniform_location("eye");
        rfl_fac = get_uniform_location("rfl_fac");
        m = get_uniform_location("m");
        spec_color = get_uniform_location("spec_color");
        spec_factors = get_uniform_location("spec_factors");
        sun = get_uniform_location("sun");
        sun_color = get_uniform_location("sun_color");
        vp = get_uniform_location("vp");

        rfl_env = get_uniform_location("rfl_env");
        CHECK_FOR_GRAPHIC_API_ERROR;
        (void)end;
    });
}

gearoenix::gles2::shader::DirectionalColoredSpeculatedBakedShadowlessOpaque::~DirectionalColoredSpeculatedBakedShadowlessOpaque()
{
    eng->add_load_function([this] {
        end_object(vtx_shd);
        end_object(frg_shd);
        end_program();
    });
}

void gearoenix::gles2::shader::DirectionalColoredSpeculatedBakedShadowlessOpaque::use()
{
    glUseProgram(shader_program);
    glEnableVertexAttribArray(vtx_att_ind);
    glEnableVertexAttribArray(nrm_att_ind);
    //////////////////////////////////////////////////////
    glVertexAttribPointer(vtx_att_ind, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glVertexAttribPointer(nrm_att_ind, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glUniform1i(rfl_fac, 0);
}

const std::vector<gearoenix::render::shader::stage::Id>& gearoenix::gles2::shader::DirectionalColoredSpeculatedBakedShadowlessOpaque::get_stages_ids() const
{
    return graphic_2_stage;
}

void gearoenix::gles2::shader::DirectionalColoredSpeculatedBakedShadowlessOpaque::set_vp(const GLfloat* data)
{
    glUniformMatrix4fv(vp, 1, GL_FALSE, data);
}

void gearoenix::gles2::shader::DirectionalColoredSpeculatedBakedShadowlessOpaque::set_m(const GLfloat* data)
{
    glUniformMatrix4fv(m, 1, GL_FALSE, data);
}

void gearoenix::gles2::shader::DirectionalColoredSpeculatedBakedShadowlessOpaque::set_sun(const GLfloat* data)
{
    glUniform3fv(sun, 1, data);
}

void gearoenix::gles2::shader::DirectionalColoredSpeculatedBakedShadowlessOpaque::set_sun_color(const GLfloat* data)
{
    glUniform3fv(sun_color, 1, data);
}

void gearoenix::gles2::shader::DirectionalColoredSpeculatedBakedShadowlessOpaque::set_ambl_color(const GLfloat* data)
{
    glUniform3fv(ambl_color, 1, data);
}

void gearoenix::gles2::shader::DirectionalColoredSpeculatedBakedShadowlessOpaque::set_spec_color(const GLfloat* data)
{
    glUniform3fv(spec_color, 1, data);
}

void gearoenix::gles2::shader::DirectionalColoredSpeculatedBakedShadowlessOpaque::set_spec_factors(const GLfloat* data)
{
    glUniform3fv(spec_factors, 1, data);
}

void gearoenix::gles2::shader::DirectionalColoredSpeculatedBakedShadowlessOpaque::set_eye(const GLfloat* data)
{
    glUniform3fv(eye, 1, data);
}

void gearoenix::gles2::shader::DirectionalColoredSpeculatedBakedShadowlessOpaque::set_rfl_fac(const GLfloat data)
{
    glUniform1f(rfl_fac, data);
}

#endif
