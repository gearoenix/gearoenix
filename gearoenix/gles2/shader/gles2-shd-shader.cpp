#include "gles2-shd-shader.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/cr-function-loader.hpp"
#include "../../system/sys-log.hpp"
#include "../engine/gles2-eng-engine.hpp"
#include <vector>

void gearoenix::gles2::shader::Shader::create_program()
{
    shader_program = glCreateProgram();
    if (shader_program == 0) {
        GXLOGF("Error creating shader program.");
    }
}

void gearoenix::gles2::shader::Shader::run()
{
	link();
	glUseProgram(shader_program);
}

void gearoenix::gles2::shader::Shader::link()
{
	GLint is_success = 0;
	glLinkProgram(shader_program);
	glGetProgramiv(shader_program, GL_LINK_STATUS, &is_success);
	if (is_success == 0) {
		GLint max_length = 0;
		glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &max_length);
		std::vector<GLchar> info_log;
		info_log.resize(max_length);
		glGetProgramInfoLog(shader_program, max_length, NULL, &(info_log[0]));
		GXLOGF("Error linking shader program: " << &(info_log[0]));
	}
	glUseProgram(shader_program);
	position_attribute_location = glGetAttribLocation(shader_program, "position");
	normal_attribute_location = glGetAttribLocation(shader_program, "normal");
	tangent_attribute_location = glGetAttribLocation(shader_program, "tangent");
	uv_attribute_location = glGetAttribLocation(shader_program, "uv");
}

void gearoenix::gles2::shader::Shader::validate()
{
	glValidateProgram(shader_program);
	GLint is_success = 0;
	glGetProgramiv(shader_program, GL_VALIDATE_STATUS, &is_success);
	if (!is_success) {
		GLint max_length = 0;
		glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &max_length);
		std::string info_log;
		info_log.resize(max_length);
		glGetProgramInfoLog(shader_program, max_length, NULL, &(info_log[0]));
		GXLOGF("Invalid shader program: " << info_log);
	}
	glUseProgram(shader_program);
}

GLuint gearoenix::gles2::shader::Shader::add_shader_to_program(const std::string& shd, const GLenum shader_type)
{
    GLuint shader_obj = glCreateShader(shader_type);
    if (shader_obj == 0) {
        GXLOGF("Error creating shader type.");
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
        GXLOGF("Error compiling shader type. Info: " << &(info_log[0]));
    }
    glAttachShader(shader_program, shader_obj);
    return shader_obj;
}

void gearoenix::gles2::shader::Shader::end_program(const GLuint shader_program)
{
    glDeleteProgram(shader_program);
}

void gearoenix::gles2::shader::Shader::end_object(const GLuint shader_object)
{
    glDeleteShader(shader_object);
}

gearoenix::gles2::shader::Shader::Shader(const std::shared_ptr<engine::Engine>& e, const core::sync::EndCaller<core::sync::EndCallerIgnore> &c)
    : e(e)
{
	e->get_function_loader()->load([c, this] {
		create_program();
	});
}

gearoenix::gles2::shader::Shader::~Shader() {
	const GLuint p = shader_program;
	const GLuint v = vertex_object;
	const GLuint f = fragment_object;
	e->get_function_loader()->load([p, v, f] {
		if(v != 0) end_object(v);
		if(f != 0) end_object(f);
		if(p != 0) end_program(p);
	});
	shader_program = 0;
	vertex_object = 0;
	fragment_object = 0;
}

GLuint gearoenix::gles2::shader::Shader::get_uniform_location(const std::string& uname) const
{
	return glGetUniformLocation(shader_program, &(uname[0]));
}
#endif
