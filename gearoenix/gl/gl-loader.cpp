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
gearoenix::gl::bind_attrib_location_fnp gearoenix::gl::Loader::bind_attrib_location;
gearoenix::gl::bind_buffer_fnp gearoenix::gl::Loader::bind_buffer;
gearoenix::gl::bind_framebuffer_fnp gearoenix::gl::Loader::bind_framebuffer;
gearoenix::gl::bind_renderbuffer_fnp gearoenix::gl::Loader::bind_renderbuffer;
gearoenix::gl::bind_texture_fnp gearoenix::gl::Loader::bind_texture;
gearoenix::gl::blend_func_fnp gearoenix::gl::Loader::blend_func;
gearoenix::gl::buffer_data_fnp gearoenix::gl::Loader::buffer_data;
gearoenix::gl::clear_color_fnp gearoenix::gl::Loader::clear_color;
gearoenix::gl::check_framebuffer_status_fnp gearoenix::gl::Loader::check_framebuffer_status;
gearoenix::gl::clear_fnp gearoenix::gl::Loader::clear;
gearoenix::gl::compile_shader_fnp gearoenix::gl::Loader::compile_shader;
gearoenix::gl::create_program_fnp gearoenix::gl::Loader::create_program;
gearoenix::gl::create_shader_fnp gearoenix::gl::Loader::create_shader;
gearoenix::gl::cull_face_fnp gearoenix::gl::Loader::cull_face;
gearoenix::gl::delete_buffers_fnp gearoenix::gl::Loader::delete_buffers;
gearoenix::gl::delete_framebuffers_fnp gearoenix::gl::Loader::delete_framebuffers;
gearoenix::gl::delete_program_fnp gearoenix::gl::Loader::delete_program;
gearoenix::gl::delete_renderbuffers_fnp gearoenix::gl::Loader::delete_renderbuffers;
gearoenix::gl::delete_shader_fnp gearoenix::gl::Loader::delete_shader;
gearoenix::gl::delete_textures_fnp gearoenix::gl::Loader::delete_textures;
gearoenix::gl::depth_mask_fnp gearoenix::gl::Loader::depth_mask;
gearoenix::gl::disable_fnp gearoenix::gl::Loader::disable;
gearoenix::gl::draw_elements_fnp gearoenix::gl::Loader::draw_elements;
gearoenix::gl::enable_fnp gearoenix::gl::Loader::enable;
gearoenix::gl::enable_vertex_attrib_array_fnp gearoenix::gl::Loader::enable_vertex_attrib_array;
gearoenix::gl::framebuffer_renderbuffer_fnp gearoenix::gl::Loader::framebuffer_renderbuffer;
gearoenix::gl::framebuffer_texture_2d_fnp gearoenix::gl::Loader::framebuffer_texture_2d;
gearoenix::gl::gen_buffers_fnp gearoenix::gl::Loader::gen_buffers;
gearoenix::gl::gen_framebuffers_fnp gearoenix::gl::Loader::gen_framebuffers;
gearoenix::gl::gen_renderbuffers_fnp gearoenix::gl::Loader::gen_renderbuffers;
gearoenix::gl::gen_textures_fnp gearoenix::gl::Loader::gen_textures;
gearoenix::gl::generate_mipmap_fnp gearoenix::gl::Loader::generate_mipmap;
gearoenix::gl::get_attrib_location_fnp gearoenix::gl::Loader::get_attrib_location;
gearoenix::gl::get_error_fnp gearoenix::gl::Loader::get_error;
gearoenix::gl::get_integerv_fnp gearoenix::gl::Loader::get_integerv;
gearoenix::gl::get_programiv_fnp gearoenix::gl::Loader::get_programiv;
gearoenix::gl::get_program_info_log_fnp gearoenix::gl::Loader::get_program_info_log;
gearoenix::gl::get_shaderiv_fnp gearoenix::gl::Loader::get_shaderiv;
gearoenix::gl::get_shader_info_log_fnp gearoenix::gl::Loader::get_shader_info_log;
gearoenix::gl::get_uniform_location_fnp gearoenix::gl::Loader::get_uniform_location;
gearoenix::gl::link_program_fnp gearoenix::gl::Loader::link_program;
gearoenix::gl::renderbuffer_storage_fnp gearoenix::gl::Loader::renderbuffer_storage;
gearoenix::gl::tex_image_2d_fnp gearoenix::gl::Loader::tex_image_2d;
gearoenix::gl::tex_parameterf_fnp gearoenix::gl::Loader::tex_parameterf;
gearoenix::gl::tex_parameterfv_fnp gearoenix::gl::Loader::tex_parameterfv;
gearoenix::gl::tex_parameteri_fnp gearoenix::gl::Loader::tex_parameteri;
gearoenix::gl::tex_parameteriv_fnp gearoenix::gl::Loader::tex_parameteriv;
gearoenix::gl::scissor_fnp gearoenix::gl::Loader::scissor;
gearoenix::gl::shader_source_fnp gearoenix::gl::Loader::shader_source;
gearoenix::gl::uniform1f_fnp gearoenix::gl::Loader::uniform1f;
gearoenix::gl::uniform1i_fnp gearoenix::gl::Loader::uniform1i;
gearoenix::gl::uniform2fv_fnp gearoenix::gl::Loader::uniform2fv;
gearoenix::gl::uniform3fv_fnp gearoenix::gl::Loader::uniform3fv;
gearoenix::gl::uniform4fv_fnp gearoenix::gl::Loader::uniform4fv;
gearoenix::gl::uniform_matrix2fv_fnp gearoenix::gl::Loader::uniform_matrix2fv;
gearoenix::gl::uniform_matrix3fv_fnp gearoenix::gl::Loader::uniform_matrix3fv;
gearoenix::gl::uniform_matrix4fv_fnp gearoenix::gl::Loader::uniform_matrix4fv;
gearoenix::gl::use_program_fnp gearoenix::gl::Loader::use_program;
gearoenix::gl::validate_program_fnp gearoenix::gl::Loader::validate_program;
gearoenix::gl::vertex_attrib_pointer_fnp gearoenix::gl::Loader::vertex_attrib_pointer;
gearoenix::gl::viewport_fnp gearoenix::gl::Loader::viewport;
#ifdef GX_USE_OPENGL_ES3
gearoenix::gl::bind_vertex_array_fnp gearoenix::gl::Loader::bind_vertex_array;
gearoenix::gl::delete_vertex_arrays_fnp gearoenix::gl::Loader::delete_vertex_arrays;
gearoenix::gl::gen_vertex_arrays_fnp gearoenix::gl::Loader::gen_vertex_arrays;
#endif

bool gearoenix::gl::Loader::load_library(const render::engine::Type::Id engine_type) noexcept
{
#ifdef GX_DEBUG_MODE
	if (engine_type != render::engine::Type::OPENGL_ES2 &&
		engine_type != render::engine::Type::OPENGL_ES3 &&
		engine_type != render::engine::Type::OPENGL_33 &&
		engine_type != render::engine::Type::OPENGL_43)
		GXLOGF("Only OpenGL API is accepted!")
#endif
#ifdef GX_USE_SDL
    if (SDL_GL_LoadLibrary(nullptr) != 0) {
        GXLOGD("Failed to load OpenGL shared library through SDL2 library loader for engine type: " << engine_type)
        return false;
    }
#else
#error "Not implemented for this platform."
#endif
#ifdef GX_USE_SDL
#define GXFUNLDF(name, fun)                                          \
    fun = reinterpret_cast<fun##_fnp>(SDL_GL_GetProcAddress(#name)); \
    if (fun == nullptr) {                                            \
        GXLOGD("Failed to load " << #name)                           \
        unload_library();                                            \
        return false;                                                \
    }
#else
#error "Not implemented for this platform."
#endif
    GXFUNLDF(glActiveTexture, active_texture)
    GXFUNLDF(glAttachShader, attach_shader)
    GXFUNLDF(glBindAttribLocation, bind_attrib_location)
    GXFUNLDF(glBindBuffer, bind_buffer)
    GXFUNLDF(glBindFramebuffer, bind_framebuffer)
    GXFUNLDF(glBindRenderbuffer, bind_renderbuffer)
    GXFUNLDF(glBindTexture, bind_texture)
    GXFUNLDF(glBlendFunc, blend_func)
    GXFUNLDF(glBufferData, buffer_data)
    GXFUNLDF(glCheckFramebufferStatus, check_framebuffer_status)
    GXFUNLDF(glClearColor, clear_color)
    GXFUNLDF(glClear, clear)
    GXFUNLDF(glCompileShader, compile_shader)
    GXFUNLDF(glCreateProgram, create_program)
    GXFUNLDF(glCreateShader, create_shader)
    GXFUNLDF(glCullFace, cull_face)
    GXFUNLDF(glDeleteBuffers, delete_buffers)
    GXFUNLDF(glDeleteFramebuffers, delete_framebuffers)
    GXFUNLDF(glDeleteProgram, delete_program)
    GXFUNLDF(glDeleteRenderbuffers, delete_renderbuffers)
    GXFUNLDF(glDeleteShader, delete_shader)
    GXFUNLDF(glDeleteTextures, delete_textures)
    GXFUNLDF(glDepthMask, depth_mask)
    GXFUNLDF(glDisable, disable)
    GXFUNLDF(glDrawElements, draw_elements)
    GXFUNLDF(glEnable, enable)
    GXFUNLDF(glEnableVertexAttribArray, enable_vertex_attrib_array)
    GXFUNLDF(glFramebufferRenderbuffer, framebuffer_renderbuffer)
    GXFUNLDF(glFramebufferTexture2D, framebuffer_texture_2d)
    GXFUNLDF(glGenBuffers, gen_buffers)
    GXFUNLDF(glGenFramebuffers, gen_framebuffers)
    GXFUNLDF(glGenRenderbuffers, gen_renderbuffers)
    GXFUNLDF(glGenTextures, gen_textures)
    GXFUNLDF(glGenerateMipmap, generate_mipmap)
    GXFUNLDF(glGetAttribLocation, get_attrib_location)
    GXFUNLDF(glGetError, get_error)
    GXFUNLDF(glGetIntegerv, get_integerv)
    GXFUNLDF(glGetProgramiv, get_programiv)
    GXFUNLDF(glGetProgramInfoLog, get_program_info_log)
    GXFUNLDF(glGetShaderiv, get_shaderiv)
    GXFUNLDF(glGetShaderInfoLog, get_shader_info_log)
    GXFUNLDF(glGetUniformLocation, get_uniform_location)
    GXFUNLDF(glLinkProgram, link_program)
    GXFUNLDF(glRenderbufferStorage, renderbuffer_storage)
    GXFUNLDF(glTexImage2D, tex_image_2d)
    GXFUNLDF(glTexParameterf, tex_parameterf)
    GXFUNLDF(glTexParameterfv, tex_parameterfv)
    GXFUNLDF(glTexParameteri, tex_parameteri)
    GXFUNLDF(glTexParameteriv, tex_parameteriv)
    GXFUNLDF(glScissor, scissor)
    GXFUNLDF(glShaderSource, shader_source)
    GXFUNLDF(glUniform1f, uniform1f)
    GXFUNLDF(glUniform1i, uniform1i)
    GXFUNLDF(glUniform2fv, uniform2fv)
    GXFUNLDF(glUniform3fv, uniform3fv)
    GXFUNLDF(glUniform4fv, uniform4fv)
    GXFUNLDF(glUniformMatrix2fv, uniform_matrix2fv)
    GXFUNLDF(glUniformMatrix3fv, uniform_matrix3fv)
    GXFUNLDF(glUniformMatrix4fv, uniform_matrix4fv)
    GXFUNLDF(glUseProgram, use_program)
    GXFUNLDF(glValidateProgram, validate_program)
    GXFUNLDF(glVertexAttribPointer, vertex_attrib_pointer)
    GXFUNLDF(glViewport, viewport)
    if (engine_type == render::engine::Type::OPENGL_ES2)
        return true;
#if defined(GX_USE_OPENGL_ES3) || defined(GX_USE_OPENGL_33) || defined(GX_USE_OPENGL_43)
	GXFUNLDF(glBindVertexArray, bind_vertex_array)
	GXFUNLDF(glDeleteVertexArrays, delete_vertex_arrays)
	GXFUNLDF(glGenVertexArrays, gen_vertex_arrays)
#endif
    return true;
}

void gearoenix::gl::Loader::unload_library() noexcept
{
#ifdef GX_USE_SDL
    SDL_GL_UnloadLibrary();
#else
#error "Not implemented for this platform."
#endif
}

void gearoenix::gl::Loader::check_for_error() noexcept
{
    switch (get_error()) {
    case GL_NO_ERROR:
        break;
    case GL_INVALID_ENUM:
        GXLOGF("Invalid enum")
    case GL_INVALID_VALUE:
        GXLOGF("Invalid value")
    case GL_INVALID_OPERATION:
        GXLOGF("Invalid operation")
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        GXLOGF("Invalid framebuffer operation error.")
    case GL_OUT_OF_MEMORY:
        GXLOGF("Out of memory error.")
    default:
        GXLOGF("Unknown error.")
    }
}
