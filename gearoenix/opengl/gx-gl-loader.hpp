#ifndef GEAROENIX_GL_LOADER_HPP
#define GEAROENIX_GL_LOADER_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "gx-gl-types.hpp"

#if defined(_WIN32) && !defined(_WIN32_WCE) && !defined(__SCITECH_SNAP__)
#define GXGL_APIENTRY __stdcall*
#else
#define GXGL_APIENTRY *
#endif

namespace gearoenix::gl {
typedef void(GXGL_APIENTRY ActiveTextureFnp)(enumerated texture);
typedef void(GXGL_APIENTRY AttachShaderFnp)(uint program, uint shader);
typedef void(GXGL_APIENTRY BindAttribLocationFnp)(uint program, uint index, const char* name);
typedef void(GXGL_APIENTRY BindBufferFnp)(enumerated, uint);
typedef void(GXGL_APIENTRY BindFramebufferFnp)(enumerated target, uint framebuffer);
typedef void(GXGL_APIENTRY BindRenderbufferFnp)(enumerated target, uint renderbuffer);
typedef void(GXGL_APIENTRY BindTextureFnp)(enumerated, uint);
typedef void(GXGL_APIENTRY BindVertexArrayFnp)(uint arr);
typedef void(GXGL_APIENTRY BlendFuncFnp)(enumerated, enumerated);
typedef void(GXGL_APIENTRY BufferDataFnp)(enumerated, sizeiptr, const void*, enumerated);
typedef enumerated(GXGL_APIENTRY CheckFramebufferStatusFnp)(enumerated target);
typedef void(GXGL_APIENTRY ClearColorFnp)(float, float, float, float);
typedef void(GXGL_APIENTRY ClearFnp)(bitfield);
typedef void(GXGL_APIENTRY CompileShaderFnp)(uint shader);
typedef uint(GXGL_APIENTRY CreateProgramFnp)();
typedef uint(GXGL_APIENTRY CreateShaderFnp)(enumerated shader_type);
typedef void(GXGL_APIENTRY CullFaceFnp)(enumerated);
typedef void(GXGL_APIENTRY DeleteBuffersFnp)(sizei n, const uint* buffers);
typedef void(GXGL_APIENTRY DeleteFramebuffersFnp)(sizei n, const uint* framebuffers);
typedef void(GXGL_APIENTRY DeleteProgramFnp)(uint program);
typedef void(GXGL_APIENTRY DeleteRenderbuffersFnp)(sizei n, const uint* renderbuffers);
typedef void(GXGL_APIENTRY DeleteShaderFnp)(uint shader);
typedef void(GXGL_APIENTRY DeleteTexturesFnp)(sizei n, const uint* textures);
typedef void(GXGL_APIENTRY DeleteVertexArraysFnp)(sizei n, const uint* arrays);
typedef void(GXGL_APIENTRY DepthMaskFnp)(boolean flag);
typedef void(GXGL_APIENTRY DisableFnp)(enumerated cap);
typedef void(GXGL_APIENTRY DrawElementsFnp)(enumerated mode, sizei count, enumerated type, const void* indices);
typedef void(GXGL_APIENTRY EnableFnp)(enumerated);
typedef void(GXGL_APIENTRY EnableVertexAttribArrayFnp)(uint index);
typedef void(GXGL_APIENTRY FramebufferRenderbufferFnp)(enumerated target, enumerated attachment, enumerated renderbuffer_target, uint renderbuffer);
typedef void(GXGL_APIENTRY FramebufferTexture2DFnp)(enumerated target, enumerated attachment, enumerated textarget, uint texture, sint level);
typedef void(GXGL_APIENTRY GenBuffersFnp)(sizei, uint*);
typedef void(GXGL_APIENTRY GenFramebuffersFnp)(sizei n, uint* framebuffers);
typedef void(GXGL_APIENTRY GenRenderbuffersFnp)(sizei n, uint* renderbuffers);
typedef void(GXGL_APIENTRY GenTexturesFnp)(sizei, uint*);
typedef void(GXGL_APIENTRY GenVertexArraysFnp)(sizei n, uint* arrays);
typedef void(GXGL_APIENTRY GenerateMipmapFnp)(enumerated target);
typedef sint(GXGL_APIENTRY GetAttribLocationFnp)(uint program, const char* name);
typedef enumerated(GXGL_APIENTRY GetErrorFnp)();
typedef void(GXGL_APIENTRY GetIntegervFnp)(enumerated pname, sint* data);
typedef void(GXGL_APIENTRY GetProgramivFnp)(uint program, enumerated pname, sint* params);
typedef void(GXGL_APIENTRY GetProgramInfoLogFnp)(uint program, sizei buf_size, sizei* length, char* info);
typedef void(GXGL_APIENTRY GetShaderivFnp)(uint shader, enumerated pname, sint* params);
typedef void(GXGL_APIENTRY GetShaderInfoLogFnp)(uint shader, sizei bufSize, sizei* length, char* info);
typedef const ubyte*(GXGL_APIENTRY GetStringFnp)(enumerated name);
typedef sint(GXGL_APIENTRY GetUniformLocationFnp)(uint program, const char* name);
typedef void(GXGL_APIENTRY LinkProgramFnp)(uint program);
typedef void(GXGL_APIENTRY ReadBufferFnp)(enumerated src);
typedef void(GXGL_APIENTRY ReadPixelsFnp)(sint x, sint y, sizei width, sizei height, enumerated format, enumerated gl_type, void* data);
typedef void(GXGL_APIENTRY RenderbufferStorageFnp)(enumerated target, enumerated internal_format, sizei img_width, sizei img_height);
typedef void(GXGL_APIENTRY TexImage2DFnp)(enumerated target, sint level, sint internal_format, sizei image_width, sizei image_height, sint border, enumerated format, enumerated data_type, const void* pixels);
typedef void(GXGL_APIENTRY TexParameterfFnp)(enumerated, enumerated, float);
typedef void(GXGL_APIENTRY TexParameterfvFnp)(enumerated, enumerated, const float*);
typedef void(GXGL_APIENTRY TexParameteriFnp)(enumerated, enumerated, sint);
typedef void(GXGL_APIENTRY TexParameterivFnp)(enumerated, enumerated, const sint*);
typedef void(GXGL_APIENTRY ScissorFnp)(sint, sint, sizei, sizei);
typedef void(GXGL_APIENTRY ShaderSourceFnp)(uint shader, sizei count, const char* const* code, const sint* length);
typedef void(GXGL_APIENTRY Uniform1fFnp)(sint location, float v0);
typedef void(GXGL_APIENTRY Uniform1fvFnp)(sint location, sizei count, const void* data);
typedef void(GXGL_APIENTRY Uniform1iFnp)(sint location, sint v0);
typedef void(GXGL_APIENTRY Uniform1ivFnp)(sint location, sizei count, const void* data);
typedef void(GXGL_APIENTRY Uniform2fvFnp)(sint location, sizei count, const void* data);
typedef void(GXGL_APIENTRY Uniform3fvFnp)(sint location, sizei count, const void* data);
typedef void(GXGL_APIENTRY Uniform4fvFnp)(sint location, sizei count, const void* data);
typedef void(GXGL_APIENTRY UniformMatrix2fvFnp)(sint location, sizei count, boolean transpose, const float* value);
typedef void(GXGL_APIENTRY UniformMatrix3fvFnp)(sint location, sizei count, boolean transpose, const float* value);
typedef void(GXGL_APIENTRY UniformMatrix4fvFnp)(sint location, sizei count, boolean transpose, const float* value);
typedef void(GXGL_APIENTRY UseProgramFnp)(uint program);
typedef void(GXGL_APIENTRY ValidateProgramFnp)(uint program);
typedef void(GXGL_APIENTRY VertexAttribPointerFnp)(uint index, sint size, enumerated type, boolean normalized, sizei stride, const void* pointer);
typedef void(GXGL_APIENTRY ViewportFnp)(sint, sint, sizei, sizei);

#define GX_GL_FUNCTION_MAP(gx_gl_function_map_arg)   \
    gx_gl_function_map_arg(ActiveTexture);           \
    gx_gl_function_map_arg(AttachShader);            \
    gx_gl_function_map_arg(BindAttribLocation);      \
    gx_gl_function_map_arg(BindBuffer);              \
    gx_gl_function_map_arg(BindFramebuffer);         \
    gx_gl_function_map_arg(BindRenderbuffer);        \
    gx_gl_function_map_arg(BindTexture);             \
    gx_gl_function_map_arg(BindVertexArray);         \
    gx_gl_function_map_arg(BlendFunc);               \
    gx_gl_function_map_arg(BufferData);              \
    gx_gl_function_map_arg(CheckFramebufferStatus);  \
    gx_gl_function_map_arg(ClearColor);              \
    gx_gl_function_map_arg(Clear);                   \
    gx_gl_function_map_arg(CompileShader);           \
    gx_gl_function_map_arg(CreateProgram);           \
    gx_gl_function_map_arg(CreateShader);            \
    gx_gl_function_map_arg(CullFace);                \
    gx_gl_function_map_arg(DeleteBuffers);           \
    gx_gl_function_map_arg(DeleteFramebuffers);      \
    gx_gl_function_map_arg(DeleteProgram);           \
    gx_gl_function_map_arg(DeleteRenderbuffers);     \
    gx_gl_function_map_arg(DeleteShader);            \
    gx_gl_function_map_arg(DeleteTextures);          \
    gx_gl_function_map_arg(DeleteVertexArrays);      \
    gx_gl_function_map_arg(DepthMask);               \
    gx_gl_function_map_arg(Disable);                 \
    gx_gl_function_map_arg(DrawElements);            \
    gx_gl_function_map_arg(Enable);                  \
    gx_gl_function_map_arg(EnableVertexAttribArray); \
    gx_gl_function_map_arg(FramebufferRenderbuffer); \
    gx_gl_function_map_arg(FramebufferTexture2D);    \
    gx_gl_function_map_arg(GenBuffers);              \
    gx_gl_function_map_arg(GenFramebuffers);         \
    gx_gl_function_map_arg(GenRenderbuffers);        \
    gx_gl_function_map_arg(GenTextures);             \
    gx_gl_function_map_arg(GenVertexArrays);         \
    gx_gl_function_map_arg(GenerateMipmap);          \
    gx_gl_function_map_arg(GetAttribLocation);       \
    gx_gl_function_map_arg(GetError);                \
    gx_gl_function_map_arg(GetIntegerv);             \
    gx_gl_function_map_arg(GetProgramiv);            \
    gx_gl_function_map_arg(GetProgramInfoLog);       \
    gx_gl_function_map_arg(GetShaderiv);             \
    gx_gl_function_map_arg(GetShaderInfoLog);        \
    gx_gl_function_map_arg(GetString);               \
    gx_gl_function_map_arg(GetUniformLocation);      \
    gx_gl_function_map_arg(LinkProgram);             \
    gx_gl_function_map_arg(ReadBuffer);              \
    gx_gl_function_map_arg(ReadPixels);              \
    gx_gl_function_map_arg(RenderbufferStorage);     \
    gx_gl_function_map_arg(TexImage2D);              \
    gx_gl_function_map_arg(TexParameterf);           \
    gx_gl_function_map_arg(TexParameterfv);          \
    gx_gl_function_map_arg(TexParameteri);           \
    gx_gl_function_map_arg(TexParameteriv);          \
    gx_gl_function_map_arg(Scissor);                 \
    gx_gl_function_map_arg(ShaderSource);            \
    gx_gl_function_map_arg(Uniform1f);               \
    gx_gl_function_map_arg(Uniform1fv);              \
    gx_gl_function_map_arg(Uniform1i);               \
    gx_gl_function_map_arg(Uniform1iv);              \
    gx_gl_function_map_arg(Uniform2fv);              \
    gx_gl_function_map_arg(Uniform3fv);              \
    gx_gl_function_map_arg(Uniform4fv);              \
    gx_gl_function_map_arg(UniformMatrix2fv);        \
    gx_gl_function_map_arg(UniformMatrix3fv);        \
    gx_gl_function_map_arg(UniformMatrix4fv);        \
    gx_gl_function_map_arg(UseProgram);              \
    gx_gl_function_map_arg(ValidateProgram);         \
    gx_gl_function_map_arg(VertexAttribPointer);     \
    gx_gl_function_map_arg(Viewport)

#define GX_GL_FUNCTION_DECL(gx_gl_function) extern gx_gl_function##Fnp gl##gx_gl_function

GX_GL_FUNCTION_MAP(GX_GL_FUNCTION_DECL);

[[nodiscard]] bool load_library() noexcept;
[[nodiscard]] void unload_library() noexcept;
}
#endif
#endif