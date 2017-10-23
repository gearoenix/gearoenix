#include "gles2-shader.hpp"
#ifdef USE_OPENGL_ES2
#include "../../system/sys-log.hpp"

void gearoenix::gles2::shader::Shader::create_program()
{
    shader_program = glCreateProgram();
    if (shader_program == 0) {
        LOGF("Error creating shader program.");
    }
}

GLuint gearoenix::gles2::shader::Shader::add_shader_to_program(const std::string& shd, const GLenum& shader_type)
{
    GLuint shader_obj = glCreateShader(shader_type);
    if (shader_obj == 0) {
        LOGF("Error creating shader type.");
    }
    const char* chtemp = shd.c_str();
    const GLint uintemp = (GLint)shd.length();
    glShaderSource(shader_obj, 1, &(chtemp), &(uintemp));
    glCompileShader(shader_obj);
    GLint success;
    glGetShaderiv(shader_obj, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint sts_size;
        glGetShaderiv(shader_obj, GL_INFO_LOG_LENGTH, &sts_size);
        std::vector<GLchar> info_log;
        info_log.resize(sts_size);
        glGetShaderInfoLog(shader_obj, sts_size, NULL, &(info_log[0]));
        LOGF("Error compiling shader type. Info: " << &(info_log[0]));
    }
    glAttachShader(shader_program, shader_obj);
    return shader_obj;
}

void gearoenix::gles2::shader::Shader::link()
{
    GLint is_success = 0;
    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &is_success);
    if (is_success == 0) {
        GLint max_length = 0;
        glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &max_length);
        std::string info_log;
        info_log.resize(max_length);
        glGetProgramInfoLog(shader_program, max_length, NULL, &(info_log[0]));
        LOGF("Error linking shader program: " << info_log);
    }
}

void gearoenix::gles2::shader::Shader::validate()
{
    GLint is_success = 0;
    glValidateProgram(shader_program);
    glGetProgramiv(shader_program, GL_VALIDATE_STATUS, &is_success);
    if (!is_success) {
        GLint max_length = 0;
        glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &max_length);
        std::string info_log;
        info_log.resize(max_length);
        glGetProgramInfoLog(shader_program, max_length, NULL, &(info_log[0]));
        LOGF("Invalid shader program: " << info_log);
    }
    glUseProgram(shader_program);
}

void gearoenix::gles2::shader::Shader::run()
{
    link();
    validate();
}

GLuint gearoenix::gles2::shader::Shader::get_uniform_location(const std::string& uname)
{
    GLuint result = glGetUniformLocation(shader_program, &(uname[0]));
    if (result == 0xFFFFFFFF) {
        UNEXPECTED;
    }
    return result;
}

void gearoenix::gles2::shader::Shader::end_program()
{
    glDeleteProgram(shader_program);
}

void gearoenix::gles2::shader::Shader::end_object(const GLuint& shader_object)
{
    glDeleteShader(shader_object);
}

gearoenix::gles2::shader::Shader::Shader()
{
    create_program();
}

gearoenix::gles2::shader::Shader::~Shader() {}

#endif
