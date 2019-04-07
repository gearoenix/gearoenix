#include "gl-loader.hpp"
#include "../system/sys-log.hpp"
#include "gl-constants.hpp"

#ifdef GX_USE_SDL
#include <SDL.h>
#else
#error "Not implemented for this platform."
#endif

gearoenix::gl::active_texture_fnp gearoenix::gl::Loader::active_texture;
gearoenix::gl::attach_shader_fnp gearoenix::gl::Loader::attach_shader;
gearoenix::gl::bind_buffer_fnp gearoenix::gl::Loader::bind_buffer;
gearoenix::gl::bind_texture_fnp gearoenix::gl::Loader::bind_texture;
gearoenix::gl::blend_func_fnp gearoenix::gl::Loader::blend_func;
gearoenix::gl::buffer_data_fnp gearoenix::gl::Loader::buffer_data;
gearoenix::gl::clear_color_fnp gearoenix::gl::Loader::clear_color;
gearoenix::gl::clear_fnp gearoenix::gl::Loader::clear;
gearoenix::gl::compile_shader_fnp gearoenix::gl::Loader::compile_shader;
gearoenix::gl::create_program_fnp gearoenix::gl::Loader::create_program;
gearoenix::gl::create_shader_fnp gearoenix::gl::Loader::create_shader;
gearoenix::gl::cull_face_fnp gearoenix::gl::Loader::cull_face;
gearoenix::gl::delete_buffers_fnp gearoenix::gl::Loader::delete_buffers;
gearoenix::gl::delete_program_fnp gearoenix::gl::Loader::delete_program;
gearoenix::gl::delete_renderbuffers_fnp gearoenix::gl::Loader::delete_renderbuffers;
gearoenix::gl::delete_shader_fnp gearoenix::gl::Loader::delete_shader;
gearoenix::gl::delete_textures_fnp gearoenix::gl::Loader::delete_textures;
gearoenix::gl::enable_fnp gearoenix::gl::Loader::enable;
gearoenix::gl::gen_buffers_fnp gearoenix::gl::Loader::gen_buffers;
gearoenix::gl::gen_textures_fnp gearoenix::gl::Loader::gen_textures;
gearoenix::gl::generate_mipmap_fnp gearoenix::gl::Loader::generate_mipmap;
gearoenix::gl::get_attrib_location_fnp gearoenix::gl::Loader::get_attrib_location;
gearoenix::gl::get_error_fnp gearoenix::gl::Loader::get_error;
gearoenix::gl::get_programiv_fnp gearoenix::gl::Loader::get_programiv;
gearoenix::gl::get_program_info_log_fnp gearoenix::gl::Loader::get_program_info_log;
gearoenix::gl::get_shaderiv_fnp gearoenix::gl::Loader::get_shaderiv;
gearoenix::gl::get_shader_info_log_fnp gearoenix::gl::Loader::get_shader_info_log;
gearoenix::gl::get_uniform_location_fnp gearoenix::gl::Loader::get_uniform_location;
gearoenix::gl::link_program_fnp gearoenix::gl::Loader::link_program;
gearoenix::gl::tex_image_2d_fnp gearoenix::gl::Loader::tex_image_2d;
gearoenix::gl::tex_parameterf_fnp gearoenix::gl::Loader::tex_parameterf;
gearoenix::gl::tex_parameterfv_fnp gearoenix::gl::Loader::tex_parameterfv;
gearoenix::gl::tex_parameteri_fnp gearoenix::gl::Loader::tex_parameteri;
gearoenix::gl::tex_parameteriv_fnp gearoenix::gl::Loader::tex_parameteriv;
gearoenix::gl::scissor_fnp gearoenix::gl::Loader::scissor;
gearoenix::gl::shader_source_fnp gearoenix::gl::Loader::shader_source;
gearoenix::gl::use_program_fnp gearoenix::gl::Loader::use_program;
gearoenix::gl::validate_program_fnp gearoenix::gl::Loader::validate_program;
gearoenix::gl::viewport_fnp gearoenix::gl::Loader::viewport;

void gearoenix::gl::Loader::load_library()
{
#ifdef GX_USE_SDL
	if (SDL_GL_LoadLibrary(nullptr) != 0) GXLOGF("Failed to load OpenGL shared library through SDL2 library loader");
#else
#error "Not implemented for this platform."
#endif
}

void gearoenix::gl::Loader::load_functions()
{
#ifdef GX_USE_SDL
#define GXFUNLDF(name, fun) fun = static_cast<fun##_fnp>(SDL_GL_GetProcAddress(#name)); if(fun == nullptr) GXLOGF("Failed to load " << #name)
#else
#error "Not implemented for this platform."
#endif
	GXFUNLDF(glActiveTexture, active_texture);
	GXFUNLDF(glAttachShader, attach_shader);
	GXFUNLDF(glBindBuffer, bind_buffer);
	GXFUNLDF(glBindTexture, bind_texture);
	GXFUNLDF(glBlendFunc, blend_func);
	GXFUNLDF(glBufferData, buffer_data);
	GXFUNLDF(glClearColor, clear_color);
	GXFUNLDF(glClear, clear);
	GXFUNLDF(glCompileShader, compile_shader);
	GXFUNLDF(glCreateProgram, create_program);
	GXFUNLDF(glCreateShader, create_shader);
	GXFUNLDF(glCullFace, cull_face);
	GXFUNLDF(glDeleteBuffers, delete_buffers);
	GXFUNLDF(glDeleteProgram, delete_program);
	GXFUNLDF(glDeleteRenderbuffers, delete_renderbuffers);
	GXFUNLDF(glDeleteShader, delete_shader);
	GXFUNLDF(glDeleteTextures, delete_textures);
	GXFUNLDF(glEnable, enable);
	GXFUNLDF(glGenBuffers, gen_buffers);
	GXFUNLDF(glGenTextures, gen_textures);
	GXFUNLDF(glGenerateMipmap, generate_mipmap);
	GXFUNLDF(glGetAttribLocation, get_attrib_location);
	GXFUNLDF(glGetError, get_error);
	GXFUNLDF(glGetProgramiv, get_programiv);
	GXFUNLDF(glGetProgramInfoLog, get_program_info_log);
	GXFUNLDF(glGetShaderiv, get_shaderiv);
	GXFUNLDF(glGetShaderInfoLog, get_shader_info_log);
	GXFUNLDF(glGetUniformLocation, get_uniform_location);
	GXFUNLDF(glLinkProgram, link_program);
	GXFUNLDF(glTexImage2D, tex_image_2d);
	GXFUNLDF(glTexParameterf, tex_parameterf);
	GXFUNLDF(glTexParameterfv, tex_parameterfv);
	GXFUNLDF(glTexParameteri, tex_parameteri);
	GXFUNLDF(glTexParameteriv, tex_parameteriv);
	GXFUNLDF(glScissor, scissor);
	GXFUNLDF(glShaderSource, shader_source);
	GXFUNLDF(glUseProgram, use_program);
	GXFUNLDF(glValidateProgram, validate_program);
	GXFUNLDF(glViewport, viewport);
}

void gearoenix::gl::Loader::check_for_error()
{
	switch (get_error()) {
	case GL_NO_ERROR:
		break;
	case GL_INVALID_ENUM:
		GXLOGF("Invalid enum");
	case GL_INVALID_VALUE:
		GXLOGF("Invalid value");
	case GL_INVALID_OPERATION:
		GXLOGF("Invalid operation");
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		GXLOGF("Invalid framebuffer operation error.");
	case GL_OUT_OF_MEMORY:
		GXLOGF("Out of memory error.");
	default:
		GXLOGF("Unknown error.");
	}
}
