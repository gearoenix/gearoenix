#include "gles2-shd-depth.hpp"
#ifdef USE_DIRECTX11
#include "../../system/sys-log.hpp"
#include "../buffer/gles2-buf-uniform.hpp"
#include "../gles2-engine.hpp"
#include "../pipeline/gles2-pip-pipeline.hpp"
#include "../texture/gles2-txt-cube.hpp"

gearoenix::gles2::shader::Depth::Resources::Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u)
    : render::material::Depth::Resources(e, pip, u)
{
}

void gearoenix::gles2::shader::Depth::Resources::bind()
{
    render::material::Depth::Uniform* data = reinterpret_cast<render::material::Depth::Uniform*>(u->get_data());
    Depth* shd = reinterpret_cast<Depth*>(pip->get_shader());
    shd->use();
    shd->set_mvp(data->mvp.data());
}

gearoenix::gles2::shader::Depth::Depth(core::Id sid, Engine* eng, std::shared_ptr<core::EndCaller> end)
    : Shader(eng, end)
    , vtx_stride(Shader::get_vertex_real_count(sid) * sizeof(GLfloat))
{
    eng->add_load_function([this, end] {
        create_program();
        GXLOGE("TODO I can disable blGXLOGEng and then i can use alpha part too.")
        const std::string pvs = "precision highp float;\n"
                                "attribute vec3 vertex;\n"
                                "uniform mat4 mvp;\n"
                                "varying float depth;\n"
                                "void main()\n"
                                "{\n"
                                "    gl_Position = mvp * vec4(vertex, 1.0);\n"
                                "    depth = (gl_Position.z * 0.5) + 0.5;\n"
                                "}\n";
        const std::string pfs = "precision highp float;\n"
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

gearoenix::gles2::shader::Depth::~Depth()
{
    eng->add_load_function([this] {
        end_object(vtx_shd);
        end_object(frg_shd);
        end_program();
    });
}

void gearoenix::gles2::shader::Depth::use()
{
    glUseProgram(shader_program);
    glEnableVertexAttribArray(vtx_att_ind);
    ////////////////////////////////////////////////////////////
    glVertexAttribPointer(vtx_att_ind, 3, GL_FLOAT, GL_FALSE, vtx_stride, (void*)0);
}

const std::vector<gearoenix::render::shader::stage::Id>& gearoenix::gles2::shader::Depth::get_stages_ids() const
{
    return graphic_2_stage;
}

void gearoenix::gles2::shader::Depth::set_mvp(const GLfloat* data)
{
    glUniformMatrix4fv(mvp, 1, GL_FALSE, data);
}
#endif
