#include "gles2-shd-directional-colored-speculated-nocube-noshadow-opaque.hpp"
#ifdef USE_OPENGL_ES2
#include "../../system/sys-log.hpp"
gearoenix::gles2::shader::DirectionalColoredSpeculatedNocubeNoshadowOpaque::DirectionalColoredSpeculatedNocubeNoshadowOpaque(Engine* eng, std::shared_ptr<core::EndCaller> end)
    : Shader(eng, end)
{
    std::string pvs = "precision highp sampler2D;\n"
                      "precision highp float;\n"
                      "attribute vec3 vertex;\n"
                      "attribute vec3 normal;\n"
                      "varying vec3 pos;\n"
                      "varying vec3 nrm;\n"
                      "uniform mat4 mvp;\n"
                      "uniform mat4 m;\n"
                      "void main()\n"
                      "{\n"
                      "    pos = (m * vec4(vertex, 1.0)).xyz;\n"
                      "    nrm = normalize((m * vec4(normal, 0.0)).xyz);\n"
                      "    gl_Position = mvp * vec4(vertex, 1.0);\n"
                      "}\n";
    std::string pfs = "precision highp sampler2D;\n"
                      "precision highp float;\n"
                      "varying vec3 pos;\n"
                      "varying vec3 nrm;\n"
                      "uniform vec3 color;\n"
                      "uniform vec3 sun;\n"
                      "uniform vec3 eye;\n"
                      "void main()\n"
                      "{\n"
                      "    float diff = -dot(nrm, sun);\n"
                      "    float spec = dot(reflect(sun, nrm), normalize(eye - pos));"
                      "    spec = smoothstep(0.8, 0.9, spec);\n"
                      "    diff = smoothstep(0.0, 0.2, diff) * (1.0 - 0.5) + 0.5;\n"
                      "    gl_FragColor = vec4(color * diff + vec3(spec), 1.0);\n"
                      "}\n";
    vtx_shd = add_shader_to_program(pvs, GL_VERTEX_SHADER);
    frg_shd = add_shader_to_program(pfs, GL_FRAGMENT_SHADER);
    run();
    vtx_att_ind = glGetAttribLocation(shader_program, "vertex");
    nrm_att_ind = glGetAttribLocation(shader_program, "normal");
    mvp = get_uniform_location("mvp");
    m = get_uniform_location("m");
    sun = get_uniform_location("sun");
    eye = get_uniform_location("eye");
    color = get_uniform_location("color");
}

gearoenix::gles2::shader::DirectionalColoredSpeculatedNocubeNoshadowOpaque::~DirectionalColoredSpeculatedNocubeNoshadowOpaque()
{
    end_object(vtx_shd);
    end_object(frg_shd);
    end_program();
}

void gearoenix::gles2::shader::DirectionalColoredSpeculatedNocubeNoshadowOpaque::use()
{
    glUseProgram(shader_program);
    glEnableVertexAttribArray(vtx_att_ind);
    glEnableVertexAttribArray(nrm_att_ind);
    //////////////////////////////////////////////////////
    glVertexAttribPointer(vtx_att_ind, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glVertexAttribPointer(nrm_att_ind, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    ///////////////////////////////////////////////////////
    const GLfloat data[] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    };
    glUniformMatrix4fv(mvp, 1, GL_FALSE, data);
    glUniformMatrix4fv(m, 1, GL_FALSE, data);
    glUniform3fv(sun, 1, data);
    glUniform3fv(eye, 1, data);
    glUniform3fv(color, 1, data);
}

const std::vector<gearoenix::render::shader::stage::Id>& gearoenix::gles2::shader::DirectionalColoredSpeculatedNocubeNoshadowOpaque::get_stages_ids() const
{
    return graphic_2_stage;
}

void gearoenix::gles2::shader::DirectionalColoredSpeculatedNocubeNoshadowOpaque::set_mvp(const GLfloat* data)
{
    glUniformMatrix4fv(mvp, 1, GL_FALSE, data);
}

#endif
