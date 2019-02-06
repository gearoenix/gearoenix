#include "gles2-shd-skybox-basic.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../system/sys-log.hpp"
#include "../buffer/gles2-buf-uniform.hpp"
#include "../gles2-engine.hpp"
#include "../pipeline/gles2-pip-pipeline.hpp"
#include "../texture/gles2-txt-cube.hpp"

gearoenix::gles2::shader::SkyboxBasic::Resources::Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u)
    : render::material::SkyboxBasic::Resources(e, pip, u)
{
}

void gearoenix::gles2::shader::SkyboxBasic::Resources::bind()
{
    render::material::SkyboxBasic::Uniform* data = static_cast<render::material::SkyboxBasic::Uniform*>(u->get_data());
    SkyboxBasic* shd = static_cast<SkyboxBasic*>(pip->get_shader());
    shd->use();
    shd->set_vp(data->vp.data());
    static_cast<texture::Cube*>(ctxt)->bind(GL_TEXTURE0);
}

gearoenix::gles2::shader::SkyboxBasic::SkyboxBasic(core::Id my_id, Engine* eng, core::sync::EndCaller<core::sync::EndCallerIgnore> end)
    : Shader(my_id, eng, end)
{
    eng->add_load_function([this, end] {
        create_program();
        const std::string pvs = "#version 100\n"
                                "precision highp samplerCube;\n"
                                "precision highp float;\n"
                                "attribute vec3 vertex;\n"
                                "varying vec3 out_pos;\n"
                                "uniform mat4 vp;\n"
                                "void main()\n"
                                "{\n"
                                "    out_pos = vertex;\n"
                                "    gl_Position = vp * vec4(vertex, 1.0);\n"
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
        link();
        glUseProgram(shader_program);
        vtx_att_ind = glGetAttribLocation(shader_program, "vertex");
        vp = get_uniform_location("vp");
        ctxt = get_uniform_location("ctxt");
        (void)end;
    });
}

gearoenix::gles2::shader::SkyboxBasic::~SkyboxBasic()
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

void gearoenix::gles2::shader::SkyboxBasic::use()
{
    glUseProgram(shader_program);
    glEnableVertexAttribArray(vtx_att_ind);
    ////////////////////////////////////////////////////////////
    glVertexAttribPointer(vtx_att_ind, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glUniform1i(ctxt, 0);
}

const std::vector<gearoenix::render::shader::stage::Id>& gearoenix::gles2::shader::SkyboxBasic::get_stages_ids() const
{
    return graphic_2_stage;
}

void gearoenix::gles2::shader::SkyboxBasic::set_vp(const GLfloat* data)
{
    glUniformMatrix4fv(vp, 1, GL_FALSE, data);
}
#endif
