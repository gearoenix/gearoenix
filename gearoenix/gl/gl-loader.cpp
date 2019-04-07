#include "gl-loader.hpp"
#include "../system/sys-log.hpp"
#include "gl-constants.hpp"

#ifdef GX_USE_SDL
#include <SDL.h> 
#else
#error "Not implemented for this platform."
#endif

void gearoenix::gl::load_functions()
{
#ifdef GX_USE_SDL
	if (SDL_GL_LoadLibrary(NULL) != 0) GXLOGF("Failed to load OpenGL shared library through SDL2 library loader");
#define GXFUNLDF(name, fun) fun = static_cast<fun##_fnp>(SDL_GL_GetProcAddress(#name)); if(fun == nullptr) GXLOGF("Failed to load " << #name)
#else
#error "Not implemented for this platform."
#endif
	GXFUNLDF(glActiveTexture, active_texture);
	GXFUNLDF(glBindBuffer, bind_buffer);
	GXFUNLDF(glBindTexture, bind_texture);
	GXFUNLDF(glBlendFunc, blend_func);
	GXFUNLDF(glBufferData, buffer_data);
	GXFUNLDF(glClearColor, clear_color);
	GXFUNLDF(glClear, clear);
	GXFUNLDF(glCreateProgram, create_program);
	GXFUNLDF(glCreateShader, create_shader);
	GXFUNLDF(glCullFace, cull_face);
	GXFUNLDF(glDeleteBuffers, delete_buffers);
	GXFUNLDF(glEnable, enable);
	GXFUNLDF(glGenBufers, gen_buffers);
	GXFUNLDF(glGenTextures, gen_textures);
	GXFUNLDF(glGenerateMipmap, generate_mipmap);
	GXFUNLDF(glGetAttribLocation, get_attrib_location);
	GXFUNLDF(glGetError, get_error);
	GXFUNLDF(glGetProgramiv, get_programiv);
	GXFUNLDF(glGetProgramInfoLog, get_program_info_log);
	GXFUNLDF(glLinkProgram, link_program);
	GXFUNLDF(glTexImage2D, tex_image_2d);
	GXFUNLDF(glTexParameterf, tex_parameterf);
	GXFUNLDF(glTexParameterfv, tex_parameterfv);
	GXFUNLDF(glTexParameteri, tex_parameteri);
	GXFUNLDF(glTexParameteriv, tex_parameteriv);
	GXFUNLDF(glScissor, scissor);
	GXFUNLDF(glUseProgram, use_program);
	GXFUNLDF(glValidateProgram, validate_program);
	GXFUNLDF(glViewport, viewport);
}

void gearoenix::gl::check_for_error()
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
