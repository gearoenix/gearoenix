#include "gles3-shd-font-colored.hpp"
#ifdef USE_OPENGL_ES3
#include "../../system/sys-log.hpp"
#include "../buffer/gles3-buf-uniform.hpp"
#include "../gles3-engine.hpp"
#include "../pipeline/gles3-pip-pipeline.hpp"
#include "../texture/gles3-txt-2d.hpp"

gearoenix::gles3::shader::FontColored::Resources::Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u)
    : render::material::FontColored::Resources(e, pip, u)
{
}

void gearoenix::gles3::shader::FontColored::Resources::bind()
{
    render::material::FontColored::Uniform* data = reinterpret_cast<render::material::FontColored::Uniform*>(u->get_data());
    FontColored* shd = reinterpret_cast<FontColored*>(pip->get_shader());
    shd->use();
    shd->set_mvp(data->mvp.data());
    shd->set_color(data->color.data());
    reinterpret_cast<texture::Texture2D*>(txt2d)->bind(GL_TEXTURE0);
}

gearoenix::gles3::shader::FontColored::FontColored(core::Id my_id, Engine* eng, core::sync::EndCaller<core::sync::EndCallerIgnore> end)
    : Shader(my_id, eng, end)
{
    eng->add_load_function([this, end] {
        create_program();
        const std::string pvs = "#version 330\n"
                                "layout(location = 0) in vec3 vertex;\n"
                                "layout(location = 1) in vec2 uv;\n"
                                "out vec2 out_uv;\n"
                                "uniform mat4 mvp;\n"
                                "void main()\n"
                                "{\n"
                                "    out_uv = uv;\n"
                                "    gl_Position = mvp * vec4(vertex, 1.0);\n"
                                "}\n";
        const std::string pfs = "#version 330\n"
                                "in vec2 out_uv;\n"
                                "out vec4 frag_color;\n"
                                "uniform vec4 color;\n"
                                "uniform sampler2D txt2d;\n"
                                "void main()\n"
                                "{\n"
                                "    vec4 txt_color = texture(txt2d, out_uv);\n"
                                "    frag_color = vec4(txt_color.xyz * color.xyz, txt_color.x * color.w);\n"
                                "}\n";
        vtx_shd = add_shader_to_program(pvs, GL_VERTEX_SHADER);
        frg_shd = add_shader_to_program(pfs, GL_FRAGMENT_SHADER);
        run();
        vtx_att_ind = glGetAttribLocation(shader_program, "vertex");
        uv_att_ind = glGetAttribLocation(shader_program, "uv");
        mvp = get_uniform_location("mvp");
        txt2d = get_uniform_location("txt2d");
        color = get_uniform_location("color");
        (void)end;
    });
}

gearoenix::gles3::shader::FontColored::~FontColored()
{
    eng->add_load_function([this] {
        end_object(vtx_shd);
        end_object(frg_shd);
        end_program();
    });
}

void gearoenix::gles3::shader::FontColored::use()
{
    glUseProgram(shader_program);
    glUniform1i(txt2d, 0);
}

const std::vector<gearoenix::render::shader::stage::Id>& gearoenix::gles3::shader::FontColored::get_stages_ids() const
{
    return graphic_2_stage;
}

void gearoenix::gles3::shader::FontColored::set_mvp(const GLfloat* data)
{
    glUniformMatrix4fv(mvp, 1, GL_FALSE, data);
}

void gearoenix::gles3::shader::FontColored::set_color(const GLfloat* data)
{
    glUniform4fv(color, 1, data);
}
#endif
