#include "gles2-shd-shader.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../gl/gl-constants.hpp"
#include "../../system/sys-log.hpp"
#include "../engine/gles2-eng-engine.hpp"
#include "../gles2.hpp"
#include <vector>

void gearoenix::gles2::shader::Shader::create_program()
{
	shader_program = gl::create_program();
	if (shader_program == 0) {
		GXLOGF("Error creating shader program.");
	}
}

void gearoenix::gles2::shader::Shader::run()
{
	link();
	gl::use_program(shader_program);
}

void gearoenix::gles2::shader::Shader::link()
{
	gl::sint is_success = 0;
	gl::link_program(shader_program);
	gl::get_programiv(shader_program, GL_LINK_STATUS, &is_success);
	if (is_success == 0) {
		gl::sint max_length = 0;
		gl::get_programiv(shader_program, GL_INFO_LOG_LENGTH, &max_length);
		std::vector<char> info_log;
		info_log.resize(max_length);
		gl::get_program_info_log(shader_program, max_length, NULL, &(info_log[0]));
		GXLOGF("Error linking shader program: " << &(info_log[0]));
	}
	gl::use_program(shader_program);
	position_attribute_location = gl::get_attrib_location(shader_program, "position");
	normal_attribute_location = gl::get_attrib_location(shader_program, "normal");
	tangent_attribute_location = gl::get_attrib_location(shader_program, "tangent");
	uv_attribute_location = gl::get_attrib_location(shader_program, "uv");
}

void gearoenix::gles2::shader::Shader::validate()
{
	gl::validate_program(shader_program);
	gl::sint is_success = 0;
	gl::get_programiv(shader_program, GL_VALIDATE_STATUS, &is_success);
	if (!is_success) {
		gl::sint max_length = 0;
		gl::get_programiv(shader_program, GL_INFO_LOG_LENGTH, &max_length);
		std::string info_log;
		info_log.resize(max_length);
		gl::get_program_info_log(shader_program, max_length, NULL, &(info_log[0]));
		GXLOGF("Invalid shader program: " << info_log);
	}
	gl::use_program(shader_program);
}

gearoenix::gl::uint gearoenix::gles2::shader::Shader::add_shader_to_program(const std::string& shd, const gl::enumerated shader_type)
{
	gl::uint shader_obj = gl::create_shader(shader_type);
	if (shader_obj == 0) {
		GXLOGF("Error creating shader type.");
	}
	const char* chtemp = shd.c_str();
	const gl::sint uintemp = static_cast<gl::sint>(shd.length());
	glShaderSource(shader_obj, 1, &(chtemp), &(uintemp));
	glCompileShader(shader_obj);
	gl::sint success;
	glGetShaderiv(shader_obj, GL_COMPILE_STATUS, &success);
	if (!success) {
		gl::sint sts_size;
		glGetShaderiv(shader_obj, GL_INFO_LOG_LENGTH, &sts_size);
		std::vector<char> info_log;
		info_log.resize(sts_size);
		glGetShaderInfoLog(shader_obj, sts_size, NULL, &(info_log[0]));
		GXLOGF("Error compiling shader type. Info: " << &(info_log[0]));
	}
	glAttachShader(shader_program, shader_obj);
	return shader_obj;
}

gearoenix::gl::uint gearoenix::gles2::shader::Shader::set_vertex_shader(const std::string & shd)
{
	return add_shader_to_program(shd, GL_VERTEX_SHADER);
}

gearoenix::gl::uint gearoenix::gles2::shader::Shader::set_fragment_shader(const std::string & shd)
{
	return add_shader_to_program(shd, GL_FRAGMENT_SHADER);
}

void gearoenix::gles2::shader::Shader::end_program(const gl::uint shader_program)
{
	glDeleteProgram(shader_program);
}

void gearoenix::gles2::shader::Shader::end_object(const gl::uint shader_object)
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
	const gl::uint p = shader_program;
	const gl::uint v = vertex_object;
	const gl::uint f = fragment_object;
	e->get_function_loader()->load([p, v, f] {
		if (v != 0) end_object(v);
		if (f != 0) end_object(f);
		if (p != 0) end_program(p);
	});
	shader_program = 0;
	vertex_object = 0;
	fragment_object = 0;
}

gearoenix::gl::uint gearoenix::gles2::shader::Shader::get_uniform_location(const std::string& uname) const
{
	return glGetUniformLocation(shader_program, &(uname[0]));
}
#endif
