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
#define GXFUNLDF(name, fun) fun = static_cast<decltype(fun)>(SDL_GL_GetProcAddress(#name)); if(fun == nullptr) GXLOGF("Failed to load " << #name)
#else
#error "Not implemented for this platform."
#endif
	GXFUNLDF(glBindBuffer, bind_buffer);
	GXFUNLDF(glBlendFunc, blend_func);
	GXFUNLDF(glBufferData, buffer_data);
	GXFUNLDF(glClearColor, clear_color);
	GXFUNLDF(glClear, clear);
	GXFUNLDF(glCreateProgram, create_program);
	GXFUNLDF(glCullFace, cull_face);
	GXFUNLDF(glDeleteBuffers, delete_buffers);
	GXFUNLDF(glEnable, enable);
	GXFUNLDF(glGetError, get_error);
	GXFUNLDF(glGenBufers, gen_buffers);
	GXFUNLDF(glScissor, scissor);
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
