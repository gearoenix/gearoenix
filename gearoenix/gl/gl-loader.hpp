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
		typedef GXGL_APICALL void(GXGL_APIENTRY attach_shader_fnp)(uint program, uint shader);
		typedef GXGL_APICALL void(GXGL_APIENTRY bind_buffer_fnp)(enumerated, uint);
		typedef GXGL_APICALL void(GXGL_APIENTRY bind_texture_fnp)(enumerated, uint);
		typedef GXGL_APICALL void(GXGL_APIENTRY blend_func_fnp)(enumerated, enumerated);
		typedef GXGL_APICALL void(GXGL_APIENTRY buffer_data_fnp)(enumerated, sizeiptr, const void *, enumerated);
		typedef GXGL_APICALL void(GXGL_APIENTRY clear_color_fnp)(float, float, float, float);
		typedef GXGL_APICALL void(GXGL_APIENTRY clear_fnp)(bitfield);
		typedef GXGL_APICALL void(GXGL_APIENTRY compile_shader_fnp)(uint shader);
		typedef GXGL_APICALL uint(GXGL_APIENTRY create_program_fnp)();
		typedef GXGL_APICALL uint(GXGL_APIENTRY create_shader_fnp)(enumerated shader_type);
		typedef GXGL_APICALL void(GXGL_APIENTRY cull_face_fnp)(enumerated);
		typedef GXGL_APICALL void(GXGL_APIENTRY delete_buffers_fnp)(sizei n, const uint *buffers);
		typedef GXGL_APICALL void(GXGL_APIENTRY delete_program_fnp)(uint program);
		typedef GXGL_APICALL void(GXGL_APIENTRY delete_renderbuffers_fnp)(sizei n, const uint *renderbuffers);
		typedef GXGL_APICALL void(GXGL_APIENTRY delete_shader_fnp)(uint shader);
		typedef GXGL_APICALL void(GXGL_APIENTRY delete_textures_fnp)(sizei n, const uint *textures);
		typedef GXGL_APICALL void(GXGL_APIENTRY enable_fnp)(enumerated);
		typedef GXGL_APICALL void(GXGL_APIENTRY gen_buffers_fnp)(sizei, uint *);
		typedef GXGL_APICALL void(GXGL_APIENTRY gen_textures_fnp)(sizei, uint *);
		typedef GXGL_APICALL void(GXGL_APIENTRY generate_mipmap_fnp)(enumerated target);
		typedef GXGL_APICALL sint(GXGL_APIENTRY get_attrib_location_fnp)(uint program, const char * name);
		typedef GXGL_APICALL enumerated(GXGL_APIENTRY get_error_fnp)();
		typedef GXGL_APICALL void(GXGL_APIENTRY get_integerv_fnp)(enumerated pname, sint *data);
		typedef GXGL_APICALL void(GXGL_APIENTRY get_programiv_fnp)(uint program, enumerated pname, sint * params);
		typedef GXGL_APICALL void(GXGL_APIENTRY get_program_info_log_fnp)(uint program, sizei buf_size, sizei *length, char * info);
		typedef GXGL_APICALL void(GXGL_APIENTRY get_shaderiv_fnp)(uint shader, enumerated pname, sint *params);
		typedef GXGL_APICALL void(GXGL_APIENTRY get_shader_info_log_fnp)(uint shader, sizei bufSize, sizei *length, char *info);
		typedef GXGL_APICALL sint(GXGL_APIENTRY get_uniform_location_fnp)(uint program, const char *name);
		typedef GXGL_APICALL void(GXGL_APIENTRY link_program_fnp)(uint program);
		typedef GXGL_APICALL void(GXGL_APIENTRY tex_image_2d_fnp)(enumerated target, sint level, sint internal_format, sizei image_width, sint image_height, sint border, enumerated format, enumerated data_type, const void *pixels);
		typedef GXGL_APICALL void(GXGL_APIENTRY tex_parameterf_fnp)(enumerated, enumerated, float);
		typedef GXGL_APICALL void(GXGL_APIENTRY tex_parameterfv_fnp)(enumerated, enumerated, const float *);
		typedef GXGL_APICALL void(GXGL_APIENTRY tex_parameteri_fnp)(enumerated, enumerated, sint);
		typedef GXGL_APICALL void(GXGL_APIENTRY tex_parameteriv_fnp)(enumerated, enumerated, const sint *);
		typedef GXGL_APICALL void(GXGL_APIENTRY scissor_fnp)(sint, sint, sizei, sizei);
		typedef GXGL_APICALL void(GXGL_APIENTRY shader_source_fnp)(uint shader, sizei count, const char *const* code, const sint *length);
		typedef GXGL_APICALL void(GXGL_APIENTRY use_program_fnp)(uint program);
		typedef GXGL_APICALL void(GXGL_APIENTRY validate_program_fnp)(uint program);
		typedef GXGL_APICALL void(GXGL_APIENTRY viewport_fnp)(sint, sint, sizei, sizei);

		class Loader {
		private:
			Loader() = delete;
			Loader(const Loader &) = delete;
		public:
			static void load_library();
			static void load_functions();
			static void check_for_error();

			static active_texture_fnp active_texture;
			static attach_shader_fnp attach_shader;
			static bind_buffer_fnp bind_buffer;
			static bind_texture_fnp bind_texture;
			static blend_func_fnp blend_func;
			static buffer_data_fnp buffer_data;
			static clear_color_fnp clear_color;
			static clear_fnp clear;
			static compile_shader_fnp compile_shader;
			static create_program_fnp create_program;
			static create_shader_fnp create_shader;
			static cull_face_fnp cull_face;
			static delete_buffers_fnp delete_buffers;
			static delete_program_fnp delete_program;
			static delete_renderbuffers_fnp delete_renderbuffers;
			static delete_shader_fnp delete_shader;
			static delete_textures_fnp delete_textures;
			static enable_fnp enable;
			static gen_buffers_fnp gen_buffers;
			static gen_textures_fnp gen_textures;
			static generate_mipmap_fnp generate_mipmap;
			static get_attrib_location_fnp get_attrib_location;
			static get_error_fnp get_error;
			static get_integerv_fnp get_integerv;
			static get_programiv_fnp get_programiv;
			static get_program_info_log_fnp get_program_info_log;
			static get_shaderiv_fnp get_shaderiv;
			static get_shader_info_log_fnp get_shader_info_log;
			static get_uniform_location_fnp get_uniform_location;
			static link_program_fnp link_program;
			static tex_image_2d_fnp tex_image_2d;
			static tex_parameterf_fnp tex_parameterf;
			static tex_parameterfv_fnp tex_parameterfv;
			static tex_parameteri_fnp tex_parameteri;
			static tex_parameteriv_fnp tex_parameteriv;
			static scissor_fnp scissor;
			static shader_source_fnp shader_source;
			static use_program_fnp use_program;
			static validate_program_fnp validate_program;
			static viewport_fnp viewport;
		};
	}
}
#endif