#include "gles3-shd-shadeless-cube-matte-nonreflective-shadowless-opaque.hpp"
#ifdef USE_OPENGL_ES3
#include "../../system/sys-log.hpp"
#include "../buffer/gles3-buf-uniform.hpp"
#include "../gles3-engine.hpp"
#include "../pipeline/gles3-pip-pipeline.hpp"
#include "../texture/gles3-txt-cube.hpp"

gearoenix::gles3::shader::ShadelessCubeMatteNonreflectiveShadowlessOpaque::Resources::Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u)
    : render::material::ShadelessCubeMatteNonreflectiveShadowlessOpaque::Resources(e, pip, u)
{
}

void gearoenix::gles3::shader::ShadelessCubeMatteNonreflectiveShadowlessOpaque::Resources::bind()
{
    render::material::ShadelessCubeMatteNonreflectiveShadowlessOpaque::Uniform* data = reinterpret_cast<render::material::ShadelessCubeMatteNonreflectiveShadowlessOpaque::Uniform*>(u->get_data());
    ShadelessCubeMatteNonreflectiveShadowlessOpaque* shd = reinterpret_cast<ShadelessCubeMatteNonreflectiveShadowlessOpaque*>(pip->get_shader());
    shd->use();
    shd->set_mvp(data->mvp.data());
    shd->set_m(data->m.data());
    reinterpret_cast<texture::Cube*>(ctxt)->bind(GL_TEXTURE0);
}

gearoenix::gles3::shader::ShadelessCubeMatteNonreflectiveShadowlessOpaque::ShadelessCubeMatteNonreflectiveShadowlessOpaque(core::Id my_id, Engine* eng, core::sync::EndCaller<core::sync::EndCallerIgnore> end)
    : Shader(my_id, eng, end)
{
    eng->add_load_function([this, end] {
        create_program();
        const std::string pvs = "#version 100\n"
                                "precision highp samplerCube;\n"
                                "precision highp float;\n"
                                "attribute vec3 vertex;\n"
                                "varying vec3 out_pos;\n"
                                "uniform mat4 mvp;\n"
                                "uniform mat4 m;\n"
                                "void main()\n"
                                "{\n"
                                "    out_pos = (m * vec4(vertex, 0.0)).xyz;\n"
                                "    gl_Position = mvp * vec4(vertex, 1.0);\n"
                                "}\n";
        const std::string pfs = "#version 100\n"
                                "precision highp samplerCube;\n"
                                "precision highp float;\n"
                                "varying vec3 out_pos;\n"
                                "uniform samplerCube ctxt;\n"
                                "void main()\n"
                                "{\n"
                                "    gl_FragColor = textureCube(ctxt, out_pos);\n"
                                "}\n";
        vtx_shd = add_shader_to_program(pvs, GL_VERTEX_SHADER);
        frg_shd = add_shader_to_program(pfs, GL_FRAGMENT_SHADER);
        run();
        vtx_att_ind = glGetAttribLocation(shader_program, "vertex");
        mvp = get_uniform_location("mvp");
        m = get_uniform_location("m");
        ctxt = get_uniform_location("ctxt");
        (void)end;
    });
}

gearoenix::gles3::shader::ShadelessCubeMatteNonreflectiveShadowlessOpaque::~ShadelessCubeMatteNonreflectiveShadowlessOpaque()
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

void gearoenix::gles3::shader::ShadelessCubeMatteNonreflectiveShadowlessOpaque::use()
{
    glUseProgram(shader_program);
    glUniform1i(ctxt, 0);
}

const std::vector<gearoenix::render::shader::stage::Id>& gearoenix::gles3::shader::ShadelessCubeMatteNonreflectiveShadowlessOpaque::get_stages_ids() const
{
    return graphic_2_stage;
}

void gearoenix::gles3::shader::ShadelessCubeMatteNonreflectiveShadowlessOpaque::set_mvp(const GLfloat* data)
{
    glUniformMatrix4fv(mvp, 1, GL_FALSE, data);
}

void gearoenix::gles3::shader::ShadelessCubeMatteNonreflectiveShadowlessOpaque::set_m(const GLfloat* data)
{
    glUniformMatrix4fv(m, 1, GL_FALSE, data);
}
#endif
