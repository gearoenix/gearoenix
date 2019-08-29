#ifndef GEAROENIX_GL_LOADER_HPP
#define GEAROENIX_GL_LOADER_HPP
#include "../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL
#include "../render/engine/rnd-eng-type.hpp"
#include "gl-types.hpp"

#if defined(_WIN32) && !defined(__SCITECH_SNAP__)
#define GXGL_APICALL __declspec(dllimport)
#elif defined(__SYMBIAN32__)
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

namespace gearoenix::gl
{
#ifdef __cplusplus
extern "C"
{
#endif
    typedef void(GXGL_APIENTRY active_texture_fnp)(enumerated texture);
    typedef void(GXGL_APIENTRY attach_shader_fnp)(uint program, uint shader);
    typedef void(GXGL_APIENTRY bind_attrib_location_fnp)(uint program, uint index,
                                                         const char *name);
    typedef void(GXGL_APIENTRY bind_buffer_fnp)(enumerated, uint);
    typedef void(GXGL_APIENTRY bind_framebuffer_fnp)(enumerated target,
                                                     uint framebuffer);
    typedef void(GXGL_APIENTRY bind_renderbuffer_fnp)(enumerated target,
                                                      uint renderbuffer);
    typedef void(GXGL_APIENTRY bind_texture_fnp)(enumerated, uint);
    typedef void(GXGL_APIENTRY blend_func_fnp)(enumerated, enumerated);
    typedef void(GXGL_APIENTRY buffer_data_fnp)(enumerated, sizeiptr, const void *,
                                                enumerated);
    typedef enumerated(GXGL_APIENTRY check_framebuffer_status_fnp)(enumerated target);
    typedef void(GXGL_APIENTRY clear_color_fnp)(float, float, float, float);
    typedef void(GXGL_APIENTRY clear_fnp)(bitfield);
    typedef void(GXGL_APIENTRY compile_shader_fnp)(uint shader);
    typedef uint(GXGL_APIENTRY create_program_fnp)();
    typedef uint(GXGL_APIENTRY create_shader_fnp)(enumerated shader_type);
    typedef void(GXGL_APIENTRY cull_face_fnp)(enumerated);
    typedef void(GXGL_APIENTRY delete_buffers_fnp)(sizei n, const uint *buffers);
    typedef void(GXGL_APIENTRY delete_framebuffers_fnp)(sizei n,
                                                        const uint *framebuffers);
    typedef void(GXGL_APIENTRY delete_program_fnp)(uint program);
    typedef void(GXGL_APIENTRY delete_renderbuffers_fnp)(sizei n,
                                                         const uint *renderbuffers);
    typedef void(GXGL_APIENTRY delete_shader_fnp)(uint shader);
    typedef void(GXGL_APIENTRY delete_textures_fnp)(sizei n, const uint *textures);
    typedef void(GXGL_APIENTRY depth_mask_fnp)(boolean flag);
    typedef void(GXGL_APIENTRY disable_fnp)(enumerated cap);
    typedef void(GXGL_APIENTRY draw_elements_fnp)(enumerated mode, sizei count,
                                                  enumerated type,
                                                  const void *indices);
    typedef void(GXGL_APIENTRY enable_fnp)(enumerated);
    typedef void(GXGL_APIENTRY enable_vertex_attrib_array_fnp)(uint index);
    typedef void(GXGL_APIENTRY framebuffer_renderbuffer_fnp)(enumerated target,
                                                             enumerated attachment,
                                                             enumerated renderbuffertarget,
                                                             uint renderbuffer);
    typedef void(GXGL_APIENTRY framebuffer_texture_2d_fnp)(enumerated target,
                                                           enumerated attachment,
                                                           enumerated textarget,
                                                           uint texture, sint level);
    typedef void(GXGL_APIENTRY gen_buffers_fnp)(sizei, uint *);
    typedef void(GXGL_APIENTRY gen_framebuffers_fnp)(sizei n, uint *framebuffers);
    typedef void(GXGL_APIENTRY gen_renderbuffers_fnp)(sizei n, uint *renderbuffers);
    typedef void(GXGL_APIENTRY gen_textures_fnp)(sizei, uint *);
    typedef void(GXGL_APIENTRY generate_mipmap_fnp)(enumerated target);
    typedef sint(GXGL_APIENTRY get_attrib_location_fnp)(uint program,
                                                        const char *name);
    typedef enumerated(GXGL_APIENTRY get_error_fnp)();
    typedef void(GXGL_APIENTRY get_integerv_fnp)(enumerated pname, sint *data);
    typedef void(GXGL_APIENTRY get_programiv_fnp)(uint program, enumerated pname,
                                                  sint *params);
    typedef void(GXGL_APIENTRY get_program_info_log_fnp)(uint program, sizei buf_size,
                                                         sizei *length, char *info);
    typedef void(GXGL_APIENTRY get_shaderiv_fnp)(uint shader, enumerated pname,
                                                 sint *params);
    typedef void(GXGL_APIENTRY get_shader_info_log_fnp)(uint shader, sizei bufSize,
                                                        sizei *length, char *info);
    typedef sint(GXGL_APIENTRY get_uniform_location_fnp)(uint program,
                                                         const char *name);
    typedef void(GXGL_APIENTRY link_program_fnp)(uint program);
    typedef void(GXGL_APIENTRY renderbuffer_storage_fnp)(enumerated target,
                                                         enumerated internal_format,
                                                         sizei img_width,
                                                         sizei img_height);
    typedef void(GXGL_APIENTRY tex_image_2d_fnp)(enumerated target, sint level,
                                                 sint internal_format,
                                                 sizei image_width, sizei image_height,
                                                 sint border, enumerated format,
                                                 enumerated data_type,
                                                 const void *pixels);
    typedef void(GXGL_APIENTRY tex_parameterf_fnp)(enumerated, enumerated, float);
    typedef void(GXGL_APIENTRY tex_parameterfv_fnp)(enumerated, enumerated,
                                                    const float *);
    typedef void(GXGL_APIENTRY tex_parameteri_fnp)(enumerated, enumerated, sint);
    typedef void(GXGL_APIENTRY tex_parameteriv_fnp)(enumerated, enumerated,
                                                    const sint *);
    typedef void(GXGL_APIENTRY scissor_fnp)(sint, sint, sizei, sizei);
    typedef void(GXGL_APIENTRY shader_source_fnp)(uint shader, sizei count,
                                                  const char *const *code,
                                                  const sint *length);
    typedef void(GXGL_APIENTRY uniform1f_fnp)(sint location, float v0);
    typedef void(GXGL_APIENTRY uniform1fv_fnp)(sint location, sizei count,
                                               const void *data);
    typedef void(GXGL_APIENTRY uniform1i_fnp)(sint location, sint v0);
    typedef void(GXGL_APIENTRY uniform1iv_fnp)(sint location, sizei count,
                                               const void *data);
    typedef void(GXGL_APIENTRY uniform2fv_fnp)(sint location, sizei count,
                                               const void *data);
    typedef void(GXGL_APIENTRY uniform3fv_fnp)(sint location, sizei count,
                                               const void *data);
    typedef void(GXGL_APIENTRY uniform4fv_fnp)(sint location, sizei count,
                                               const void *data);
    typedef void(GXGL_APIENTRY uniform_matrix2fv_fnp)(sint location, sizei count,
                                                      boolean transpose,
                                                      const float *value);
    typedef void(GXGL_APIENTRY uniform_matrix3fv_fnp)(sint location, sizei count,
                                                      boolean transpose,
                                                      const float *value);
    typedef void(GXGL_APIENTRY uniform_matrix4fv_fnp)(sint location, sizei count,
                                                      boolean transpose,
                                                      const float *value);
    typedef void(GXGL_APIENTRY use_program_fnp)(uint program);
    typedef void(GXGL_APIENTRY validate_program_fnp)(uint program);
    typedef void(GXGL_APIENTRY vertex_attrib_pointer_fnp)(uint index, sint size,
                                                          enumerated type,
                                                          boolean normalized,
                                                          sizei stride,
                                                          const void *pointer);
    typedef void(GXGL_APIENTRY viewport_fnp)(sint, sint, sizei, sizei);
#ifdef GX_USE_OPENGL_CLASS_3
    typedef void(GXGL_APIENTRY bind_vertex_array_fnp)(uint arr);
    typedef void(GXGL_APIENTRY delete_vertex_arrays_fnp)(sizei n, const uint *arrays);
    typedef void(GXGL_APIENTRY gen_vertex_arrays_fnp)(sizei n, uint *arrays);
    typedef void(GXGL_APIENTRY read_buffer)(enumerated src);
#endif
#ifdef __cplusplus
}
#endif
class Loader
{
private:
public:
    Loader() = delete;
    Loader(const Loader &) = delete;
    static bool load_library(render::engine::Type engine_type) noexcept;
    static void unload_library() noexcept;
    static void check_for_error() noexcept;

    static GXGL_APICALL active_texture_fnp active_texture;
    static GXGL_APICALL attach_shader_fnp attach_shader;
    static GXGL_APICALL bind_attrib_location_fnp bind_attrib_location;
    static GXGL_APICALL bind_buffer_fnp bind_buffer;
    static GXGL_APICALL bind_framebuffer_fnp bind_framebuffer;
    static GXGL_APICALL bind_renderbuffer_fnp bind_renderbuffer;
    static GXGL_APICALL bind_texture_fnp bind_texture;
    static GXGL_APICALL blend_func_fnp blend_func;
    static GXGL_APICALL buffer_data_fnp buffer_data;
    static GXGL_APICALL check_framebuffer_status_fnp check_framebuffer_status;
    static GXGL_APICALL clear_color_fnp clear_color;
    static GXGL_APICALL clear_fnp clear;
    static GXGL_APICALL compile_shader_fnp compile_shader;
    static GXGL_APICALL create_program_fnp create_program;
    static GXGL_APICALL create_shader_fnp create_shader;
    static GXGL_APICALL cull_face_fnp cull_face;
    static GXGL_APICALL delete_buffers_fnp delete_buffers;
    static GXGL_APICALL delete_framebuffers_fnp delete_framebuffers;
    static GXGL_APICALL delete_program_fnp delete_program;
    static GXGL_APICALL delete_renderbuffers_fnp delete_renderbuffers;
    static GXGL_APICALL delete_shader_fnp delete_shader;
    static GXGL_APICALL delete_textures_fnp delete_textures;
    static GXGL_APICALL depth_mask_fnp depth_mask;
    static GXGL_APICALL disable_fnp disable;
    static GXGL_APICALL draw_elements_fnp draw_elements;
    static GXGL_APICALL enable_fnp enable;
    static GXGL_APICALL enable_vertex_attrib_array_fnp enable_vertex_attrib_array;
    static GXGL_APICALL framebuffer_renderbuffer_fnp framebuffer_renderbuffer;
    static GXGL_APICALL framebuffer_texture_2d_fnp framebuffer_texture_2d;
    static GXGL_APICALL gen_buffers_fnp gen_buffers;
    static GXGL_APICALL gen_framebuffers_fnp gen_framebuffers;
    static GXGL_APICALL gen_renderbuffers_fnp gen_renderbuffers;
    static GXGL_APICALL gen_textures_fnp gen_textures;
    static GXGL_APICALL generate_mipmap_fnp generate_mipmap;
    static GXGL_APICALL get_attrib_location_fnp get_attrib_location;
    static GXGL_APICALL get_error_fnp get_error;
    static GXGL_APICALL get_integerv_fnp get_integerv;
    static GXGL_APICALL get_programiv_fnp get_programiv;
    static GXGL_APICALL get_program_info_log_fnp get_program_info_log;
    static GXGL_APICALL get_shaderiv_fnp get_shaderiv;
    static GXGL_APICALL get_shader_info_log_fnp get_shader_info_log;
    static GXGL_APICALL get_uniform_location_fnp get_uniform_location;
    static GXGL_APICALL link_program_fnp link_program;
    static GXGL_APICALL renderbuffer_storage_fnp renderbuffer_storage;
    static GXGL_APICALL tex_image_2d_fnp tex_image_2d;
    static GXGL_APICALL tex_parameterf_fnp tex_parameterf;
    static GXGL_APICALL tex_parameterfv_fnp tex_parameterfv;
    static GXGL_APICALL tex_parameteri_fnp tex_parameteri;
    static GXGL_APICALL tex_parameteriv_fnp tex_parameteriv;
    static GXGL_APICALL scissor_fnp scissor;
    static GXGL_APICALL shader_source_fnp shader_source;
    static GXGL_APICALL uniform1f_fnp uniform1f;
    static GXGL_APICALL uniform1fv_fnp uniform1fv;
    static GXGL_APICALL uniform1i_fnp uniform1i;
    static GXGL_APICALL uniform1iv_fnp uniform1iv;
    static GXGL_APICALL uniform2fv_fnp uniform2fv;
    static GXGL_APICALL uniform3fv_fnp uniform3fv;
    static GXGL_APICALL uniform4fv_fnp uniform4fv;
    static GXGL_APICALL uniform_matrix2fv_fnp uniform_matrix2fv;
    static GXGL_APICALL uniform_matrix3fv_fnp uniform_matrix3fv;
    static GXGL_APICALL uniform_matrix4fv_fnp uniform_matrix4fv;
    static GXGL_APICALL use_program_fnp use_program;
    static GXGL_APICALL validate_program_fnp validate_program;
    static GXGL_APICALL vertex_attrib_pointer_fnp vertex_attrib_pointer;
    static GXGL_APICALL viewport_fnp viewport;
#ifdef GX_USE_OPENGL_CLASS_3
    static GXGL_APICALL bind_vertex_array_fnp bind_vertex_array;
    static GXGL_APICALL delete_vertex_arrays_fnp delete_vertex_arrays;
    static GXGL_APICALL gen_vertex_arrays_fnp gen_vertex_arrays;
#endif
};
} // namespace gearoenix::gl
#endif
#endif