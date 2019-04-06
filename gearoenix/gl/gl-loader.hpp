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
#define GXGL_APIENTRY __stdcall
#else
#define GXGL_APIENTRY
#endif

namespace gearoenix {
	namespace gl {
		typedef GXGL_APICALL void(*GXGL_APIENTRY bind_buffer_fnp)(enumerated, uint);
		typedef GXGL_APICALL void(*GXGL_APIENTRY blend_func_fnp)(enumerated, enumerated);
		typedef GXGL_APICALL void(*GXGL_APIENTRY buffer_data_fnp)(enumerated, sizeiptr, const void *, enumerated);
		typedef GXGL_APICALL void(*GXGL_APIENTRY clear_color_fnp)(float, float, float, float);
		typedef GXGL_APICALL void(*GXGL_APIENTRY clear_fnp)(bitfield);
		typedef GXGL_APICALL uint(*GXGL_APIENTRY create_program_fnp)();
		typedef GXGL_APICALL void(*GXGL_APIENTRY cull_face_fnp)(enumerated);
		typedef GXGL_APICALL void(*GXGL_APIENTRY delete_buffers_fnp)(sizei n, const uint *buffers);
		typedef GXGL_APICALL void(*GXGL_APIENTRY enable_fnp)(enumerated);
		typedef GXGL_APICALL void(*GXGL_APIENTRY gen_buffers_fnp)(sizei, uint *);
		typedef GXGL_APICALL enumerated(*GXGL_APIENTRY get_error_fnp)();
		typedef GXGL_APICALL void(*GXGL_APIENTRY scissor_fnp)(sint, sint, sizei, sizei);
		typedef GXGL_APICALL void(*GXGL_APIENTRY viewport_fnp)(sint, sint, sizei, sizei);

		void load_functions();
		void check_for_error();
		
		bind_buffer_fnp bind_buffer;
		blend_func_fnp blend_func;
		buffer_data_fnp buffer_data;
		clear_color_fnp clear_color;
		clear_fnp clear;
		create_program_fnp create_program;
		cull_face_fnp cull_face;
		delete_buffers_fnp delete_buffers;
		enable_fnp enable;
		gen_buffers_fnp gen_buffers;
		get_error_fnp get_error;
		scissor_fnp scissor;
		viewport_fnp viewport;
	}
}
#endif