#include "gles3-shd-depth.hpp"
#ifdef USE_OPENGL_ES3
#include "../../system/sys-log.hpp"
#include "../buffer/gles3-buf-uniform.hpp"
#include "../gles3-engine.hpp"
#include "../pipeline/gles3-pip-pipeline.hpp"
#include "../texture/gles3-txt-cube.hpp"

gearoenix::gles3::shader::Depth::Resources::Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u)
    : render::material::Depth::Resources(e, pip, u)
{
}

void gearoenix::gles3::shader::Depth::Resources::bind()
{
    render::material::Depth::Uniform* data = static_cast<render::material::Depth::Uniform*>(u->get_data());
    Depth* shd = static_cast<Depth*>(pip->get_shader());
    shd->use();
    shd->set_mvp(data->mvp.data());
}

gearoenix::gles3::shader::Depth::Depth(core::Id sid, Engine* eng, core::sync::EndCaller<core::sync::EndCallerIgnore> end)
    : Shader(sid, eng, end)
    , vtx_stride(Shader::get_vertex_real_count(sid) * sizeof(GLfloat))
{
    eng->add_load_function([this, end] {
        create_program();
        GXLOGE("TODO I can disable blending and then i can use alpha part too.")
        const std::string pvs = "#version 100\n"
                                "precision highp float;\n"
                                "attribute vec3 vertex;\n"
                                "uniform mat4 mvp;\n"
                                "varying float depth;\n"
                                "void main()\n"
                                "{\n"
                                "    gl_Position = mvp * vec4(vertex, 1.0);\n"
                                "    depth = (gl_Position.z * 0.5) + 0.5;\n"
                                "}\n";
        const std::string pfs = "#version 100\n"
                                "precision highp float;\n"
                                "varying float depth;\n"
                                "void main()\n"
                                "{\n"
                                "    float y = fract(depth * 256.0);\n"
                                "    gl_FragColor = vec4(depth, y, 0.0, 1.0);\n"
                                "}\n";
        vtx_shd = add_shader_to_program(pvs, GL_VERTEX_SHADER);
        frg_shd = add_shader_to_program(pfs, GL_FRAGMENT_SHADER);
        run();
        vtx_att_ind = glGetAttribLocation(shader_program, "vertex");
        mvp = get_uniform_location("mvp");
        (void)end;
    });
}

gearoenix::gles3::shader::Depth::~Depth()
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

void gearoenix::gles3::shader::Depth::use()
{
    glUseProgram(shader_program);
}

const std::vector<gearoenix::render::shader::stage::Id>& gearoenix::gles3::shader::Depth::get_stages_ids() const
{
    return graphic_2_stage;
}

void gearoenix::gles3::shader::Depth::set_mvp(const GLfloat* data)
{
    glUniformMatrix4fv(mvp, 1, GL_FALSE, data);
}
#endif
