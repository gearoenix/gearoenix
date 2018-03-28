#include "gles2-shd-skybox-basic.hpp"
#ifdef USE_OPENGL_ES2
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
    render::material::SkyboxBasic::Uniform* data = reinterpret_cast<render::material::SkyboxBasic::Uniform*>(u->get_data());
    SkyboxBasic* shd = reinterpret_cast<SkyboxBasic*>(pip->get_shader());
    shd->use();
    shd->set_vp(data->vp.data());
    reinterpret_cast<texture::Cube*>(ctxt)->bind(GL_TEXTURE0);
}

gearoenix::gles2::shader::SkyboxBasic::SkyboxBasic(core::Id my_id, Engine* eng, core::EndCaller<core::EndCallerIgnore> end)
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
    eng->add_load_function([this] {
        end_object(vtx_shd);
        end_object(frg_shd);
        end_program();
    });
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
