#include "gx-gl-loader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/macro/gx-cr-mcr-stringifier.hpp"
#include "../platform/gx-plt-log.hpp"
#include "gx-gl-constants.hpp"

#ifdef GX_PLATFORM_INTERFACE_SDL2
#include <SDL.h>
#elif defined(GX_PLATFORM_INTERFACE_ANDROID)
#include <EGL/egl.h>
#else
#error "Not implemented for this platform."
#endif

#define GX_GL_FUNCTION_DEF(gx_gl_function) gearoenix::gl::gx_gl_function##Fnp gearoenix::gl::gl##gx_gl_function = nullptr

GX_GL_FUNCTION_MAP(GX_GL_FUNCTION_DEF);

static bool is_loaded = false;

bool gearoenix::gl::load_library() noexcept
{
#define GX_GL_FUNCTION_LOAD_CHECK(name)                                  \
    if (nullptr == gl##name) {                                           \
        GX_LOG_D(GX_STRINGIFY(gl##name) " function pointer not found."); \
        return false;                                                    \
    }

    if (is_loaded) {
        GX_GL_FUNCTION_MAP(GX_GL_FUNCTION_LOAD_CHECK)
        return true;
    }
    is_loaded = true;

#ifdef GX_PLATFORM_INTERFACE_SDL2

    if (SDL_GL_LoadLibrary(nullptr) != 0) {
        GX_LOG_D("Failed to load OpenGL shared library through SDL2 library loader.");
        return false;
    }

#define GX_GL_FUNCTION_LOAD(name)                                                          \
    gl##name = reinterpret_cast<name##Fnp>(SDL_GL_GetProcAddress(GX_STRINGIFY(gl##name))); \
    GX_GL_FUNCTION_LOAD_CHECK(name)

    GX_GL_FUNCTION_MAP(GX_GL_FUNCTION_LOAD)
#elif defined(GX_PLATFORM_INTERFACE_ANDROID)
#define GX_GL_FUNCTION_LOAD(name)                                                      \
    gl##name = reinterpret_cast<name##Fnp>(eglGetProcAddress(GX_STRINGIFY(gl##name))); \
    GX_GL_FUNCTION_LOAD_CHECK(name)

    GX_GL_FUNCTION_MAP(GX_GL_FUNCTION_LOAD)
#else
#error "Not implemented for this platform."
#endif
    auto extensions = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
    GX_LOG_D(extensions);
    return true;
}

void gearoenix::gl::unload_library() noexcept
{
    is_loaded = false;

#ifdef GX_PLATFORM_INTERFACE_SDL2
    SDL_GL_UnloadLibrary();
#endif

#define GX_GL_FUNCTION_UNLOAD(name) gl##name = nullptr

    GX_GL_FUNCTION_MAP(GX_GL_FUNCTION_UNLOAD);
}

#endif