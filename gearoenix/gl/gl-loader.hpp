#ifndef GL_LOADER_HPP
#define GL_LOADER_HPP
#include "../core/cr-build-configuration.hpp"
#include "gl-types.hpp"

#if defined(_WIN32) && !defined(__SCITECH_SNAP__)
#define GXGL_APICALL __declspec(dllimport)
#elif defined (__SYMBIAN32__)
#define GXGL_APICALL IMPORT_C
#elif defined(ANDROID)
#define GXGL_APICALL __attribute__((visibility("default")))
#else
#define GXGL_APICALL
#endif

#if defined(_WIN32) && !defined(_WIN32_WCE) && !defined(__SCITECH_SNAP__)
#define GXGL_APIENTRY __stdcall *
#else
#define GXGL_APIENTRY *
#endif

namespace gearoenix {
	namespace gl {
		typedef GXGL_APICALL void(GXGL_APIENTRY active_texture_fnp)(enumerated texture);
		typedef GXGL_APICALL void(GXGL_APIENTRY bind_buffer_fnp)(enumerated, uint);
		typedef GXGL_APICALL void(GXGL_APIENTRY bind_texture_fnp)(enumerated, uint);
		typedef GXGL_APICALL void(GXGL_APIENTRY blend_func_fnp)(enumerated, enumerated);
		typedef GXGL_APICALL void(GXGL_APIENTRY buffer_data_fnp)(enumerated, sizeiptr, const void *, enumerated);
		typedef GXGL_APICALL void(GXGL_APIENTRY clear_color_fnp)(float, float, float, float);
		typedef GXGL_APICALL void(GXGL_APIENTRY clear_fnp)(bitfield);
		typedef GXGL_APICALL uint(GXGL_APIENTRY create_program_fnp)();
		typedef GXGL_APICALL uint(GXGL_APIENTRY create_shader_fnp)(enumerated shader_type);
		typedef GXGL_APICALL void(GXGL_APIENTRY cull_face_fnp)(enumerated);
		typedef GXGL_APICALL void(GXGL_APIENTRY delete_buffers_fnp)(sizei n, const uint *buffers);
		typedef GXGL_APICALL void(GXGL_APIENTRY enable_fnp)(enumerated);
		typedef GXGL_APICALL void(GXGL_APIENTRY gen_buffers_fnp)(sizei, uint *);
		typedef GXGL_APICALL void(GXGL_APIENTRY gen_textures_fnp)(sizei, uint *);
		typedef GXGL_APICALL void(GXGL_APIENTRY generate_mipmap_fnp)(enumerated target);
		typedef GXGL_APICALL sint(GXGL_APIENTRY get_attrib_location_fnp)(uint program, const char * name);
		typedef GXGL_APICALL enumerated(GXGL_APIENTRY get_error_fnp)();
		typedef GXGL_APICALL void(GXGL_APIENTRY get_programiv_fnp)(uint program, enumerated pname, sint * params);
		typedef GXGL_APICALL void(GXGL_APIENTRY get_program_info_log_fnp)(uint program, sizei buf_size, sizei *length, char * info);
		typedef GXGL_APICALL void(GXGL_APIENTRY link_program_fnp)(uint program);
		typedef GXGL_APICALL void(GXGL_APIENTRY tex_image_2d_fnp)(enumerated target, sint level, sint internal_format, sizei image_width, sint image_height, sint border, enumerated format, enumerated data_type, const void *pixels);
		typedef GXGL_APICALL void(GXGL_APIENTRY tex_parameterf_fnp)(enumerated, enumerated, float);
		typedef GXGL_APICALL void(GXGL_APIENTRY tex_parameterfv_fnp)(enumerated, enumerated, const float *);
		typedef GXGL_APICALL void(GXGL_APIENTRY tex_parameteri_fnp)(enumerated, enumerated, sint);
		typedef GXGL_APICALL void(GXGL_APIENTRY tex_parameteriv_fnp)(enumerated, enumerated, const sint *);
		typedef GXGL_APICALL void(GXGL_APIENTRY scissor_fnp)(sint, sint, sizei, sizei);
		typedef GXGL_APICALL void(GXGL_APIENTRY use_program_fnp)(uint program);
		typedef GXGL_APICALL void(GXGL_APIENTRY validate_program_fnp)(uint program);
		typedef GXGL_APICALL void(GXGL_APIENTRY viewport_fnp)(sint, sint, sizei, sizei);

		void load_functions();
		void check_for_error();
		
		active_texture_fnp active_texture;
		bind_buffer_fnp bind_buffer;
		bind_texture_fnp bind_texture;
		blend_func_fnp blend_func;
		buffer_data_fnp buffer_data;
		clear_color_fnp clear_color;
		clear_fnp clear;
		create_program_fnp create_program;
		create_shader_fnp create_shader;
		cull_face_fnp cull_face;
		delete_buffers_fnp delete_buffers;
		enable_fnp enable;
		gen_buffers_fnp gen_buffers;
		gen_textures_fnp gen_textures;
		generate_mipmap_fnp generate_mipmap;
		get_attrib_location_fnp get_attrib_location;
		get_error_fnp get_error;
		get_programiv_fnp get_programiv;
		get_program_info_log_fnp get_program_info_log;
		link_program_fnp link_program;
		tex_image_2d_fnp tex_image_2d;
		tex_parameterf_fnp tex_parameterf;
		tex_parameterfv_fnp tex_parameterfv;
		tex_parameteri_fnp tex_parameteri;
		tex_parameteriv_fnp tex_parameteriv;
		scissor_fnp scissor;
		use_program_fnp use_program;
		validate_program_fnp validate_program;
		viewport_fnp viewport;
	}
}
#endif