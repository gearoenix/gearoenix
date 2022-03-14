#include "gx-gl-loader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/macro/gx-cr-mcr-stringifier.hpp"
#include "../platform/gx-plt-log.hpp"

#ifdef GX_PLATFORM_INTERFACE_SDL2
#include <SDL.h>
#elif defined(GX_ANDROID)
#include <EGL/egl.h>
#else
#error "Not implemented for this platform."
#endif

#define GX_GL_FUNCTION_DEF(gx_gl_function) gearoenix::gl::gx_gl_function##Fnp gearoenix::gl::gl##gx_gl_function = nullptr;

GX_GL_FUNCTION_MAP(GX_GL_FUNCTION_DEF)

bool gearoenix::gl::load_library() noexcept
{
    static bool is_loaded = false;
    if (is_loaded)
        return false;
    is_loaded = true;
#ifdef GX_PLATFORM_INTERFACE_SDL2
    if (SDL_GL_LoadLibrary(nullptr) != 0) {
        GX_LOG_D("Failed to load OpenGL shared library through SDL2 library loader.")
        return true;
    }
#define GX_GL_FUNCTION_LOAD(name) gl##name = reinterpret_cast<name##Fnp>(SDL_GL_GetProcAddress(GX_STRINGIFY(gl##name)));
    GX_GL_FUNCTION_MAP(GX_GL_FUNCTION_LOAD)
#elif defined(GX_ANDROID)
#define GXFUNLDF(name, fun)                                      \
    fun = reinterpret_cast<fun##_fnp>(eglGetProcAddress(#name)); \
    if (fun == nullptr) {                                        \
        GXLOGD("Failed to load " << #name)                       \
        unload_library();                                        \
        return false;                                            \
    }
#else
#error "Not implemented for this platform."
#endif
    auto fff = SDL_GL_GetProcAddress("glActiveTexture");
    return false;
}

#endif