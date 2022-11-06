#include "gx-gl-shader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../gx-gl-engine.hpp"
#include <vector>

void gearoenix::gl::shader::Shader::run() noexcept
{
    link();
    glUseProgram(shader_program);
}

void gearoenix::gl::shader::Shader::link() const noexcept
{
    sint is_success = 0;
    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &is_success);
    sint max_length = 0;
    glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &max_length);
    std::string info_log;
    info_log.resize(static_cast<std::size_t>(max_length));
    glGetProgramInfoLog(shader_program, static_cast<sizei>(max_length), nullptr, &(info_log[0]));
    if (is_success == 0) {
        info_log[max_length - 1] = '\n';
        GX_LOG_F("Error linking shader program: " << info_log);
    } else if (!info_log.empty()) {
        info_log[max_length - 1] = '\n';
        GX_LOG_D("Shader linking log is: " << info_log);
    }
    glUseProgram(shader_program);
}

void gearoenix::gl::shader::Shader::validate() const noexcept
{
    glValidateProgram(shader_program);
    sint is_success = 0;
    glGetProgramiv(shader_program, GL_VALIDATE_STATUS, &is_success);
    sint max_length = 0;
    glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &max_length);
    std::string info_log;
    info_log.resize(static_cast<std::size_t>(max_length));
    glGetProgramInfoLog(shader_program, static_cast<sizei>(max_length), nullptr, &(info_log[0]));
    if (!is_success) {
        info_log[max_length - 1] = '\n';
        GX_LOG_F("Invalid shader program: " << info_log);
    } else if (!info_log.empty()) {
        info_log[max_length - 1] = '\n';
        GX_LOG_D("Shader program log is: " << info_log);
    }
    glUseProgram(shader_program);
}

gearoenix::gl::uint gearoenix::gl::shader::Shader::add_shader_to_program(const std::string& shd, const enumerated shader_type) const noexcept
{
    uint shader_obj = glCreateShader(shader_type);
    if (shader_obj == 0) {
        GX_LOG_F("Error creating shader type.");
    }
    const char* const shader_src_ptr = shd.c_str();
    const auto shader_src_len = static_cast<sint>(shd.length());
    glShaderSource(shader_obj, 1, &(shader_src_ptr), &(shader_src_len));
    glCompileShader(shader_obj);
    sint success;
    glGetShaderiv(shader_obj, GL_COMPILE_STATUS, &success);
    sint sts_size;
    glGetShaderiv(shader_obj, GL_INFO_LOG_LENGTH, &sts_size);
    std::string info_log;
    info_log.resize(static_cast<std::size_t>(sts_size));
    glGetShaderInfoLog(shader_obj, static_cast<sizei>(sts_size), nullptr, &(info_log[0]));
    if (!success) {
        info_log[sts_size - 1] = '\n';
        GX_LOG_F("Error compiling shader. Info: " << info_log << ", shader source: \n"
                                                  << shd);
    } else if (!info_log.empty()) {
        info_log[sts_size - 1] = '\n';
        GX_LOG_D("Shader compiler log is: " << info_log);
    }
    glAttachShader(shader_program, shader_obj);
    return shader_obj;
}

void gearoenix::gl::shader::Shader::set_vertex_shader(const std::string& shd) noexcept
{
    vertex_object = add_shader_to_program(shd, GL_VERTEX_SHADER);
}

void gearoenix::gl::shader::Shader::set_fragment_shader(const std::string& shd) noexcept
{
    fragment_object = add_shader_to_program(shd, GL_FRAGMENT_SHADER);
}

gearoenix::gl::shader::Shader::Shader(Engine& e) noexcept
    : shader_program(glCreateProgram())
    , e(e)
{
    if (shader_program == 0) {
        GX_LOG_F("Error creating shader program.");
    }
}

gearoenix::gl::shader::Shader::Shader(Shader&& o) noexcept
    : shader_program(o.shader_program)
    , e(o.e)
    , vertex_object(o.vertex_object)
    , fragment_object(o.fragment_object)
{
    o.shader_program = static_cast<uint>(-1);
    o.vertex_object = static_cast<uint>(-1);
    o.fragment_object = static_cast<uint>(-1);
}

gearoenix::gl::shader::Shader& gearoenix::gl::shader::Shader::operator=(Shader&& o) noexcept
{
    shader_program = o.shader_program;
    vertex_object = o.vertex_object;
    fragment_object = o.fragment_object;
    o.shader_program = static_cast<uint>(-1);
    o.vertex_object = static_cast<uint>(-1);
    o.fragment_object = static_cast<uint>(-1);
    return *this;
}

gearoenix::gl::shader::Shader::~Shader() noexcept
{
    if (static_cast<uint>(-1) != vertex_object)
        glDeleteShader(vertex_object);
    if (static_cast<uint>(-1) != fragment_object)
        glDeleteShader(fragment_object);
    if (static_cast<uint>(-1) != shader_program)
        glDeleteProgram(shader_program);
}

gearoenix::gl::sint gearoenix::gl::shader::Shader::get_uniform_location(const std::string& uname) const noexcept
{
    return glGetUniformLocation(shader_program, &(uname[0]));
}

void gearoenix::gl::shader::Shader::bind(uint& current_shader) const noexcept
{
    glUseProgram(shader_program);
    current_shader = shader_program;
}

#endif