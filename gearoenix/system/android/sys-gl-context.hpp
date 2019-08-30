#ifndef GEAROENIX_SYSTEM_ANDROID_GL_CONTEXT_HPP
#define GEAROENIX_SYSTEM_ANDROID_GL_CONTEXT_HPP
#include "../../core/cr-build-configuration.hpp"
#if defined(GX_IN_ANDROID) && !defined(GX_USE_SDL) && defined(GX_USE_OPENGL)
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include "../../core/cr-static.hpp"

namespace gearoenix::system {
    class GlContext {
    GX_GET_VAL_PRV(int, screen_width, -1)
    GX_GET_VAL_PRV(int, screen_height, -1)
    GX_GET_VAL_PRV(int, depth_size, -1)
    GX_GET_VAL_PRV(int, samples_size, -1)
    GX_GET_VAL_PRV(bool, es3_supported, false)
    public:
        enum struct State {
            RUNNING,
            TERMINATED,
        };
    private:
        ANativeWindow* window = nullptr;
        EGLDisplay display = EGL_NO_DISPLAY;
        EGLSurface surface = EGL_NO_SURFACE;
        EGLContext context = EGL_NO_CONTEXT;
        EGLConfig config = nullptr;

        bool gles_initialized = false;
        bool egl_context_initialized = false;
        bool context_valid = false;

        void init_gles() noexcept;
        void terminate() noexcept;
        bool check_surface(EGLint opengl_version, EGLint depth_size, EGLint samples_size) noexcept;
        void init_egl_surface() noexcept;
        void init_egl_context() noexcept;
    public:
        GlContext() noexcept = default;
        ~GlContext() noexcept;
        GlContext( GlContext const& ) = delete;
        void operator=( GlContext const& ) = delete;

        void init(ANativeWindow* window) noexcept ;
        State swap() noexcept ;
        void invalidate() noexcept ;
        void suspend() noexcept ;
        void resume(ANativeWindow* window ) noexcept ;
    };
}
#endif
#endif
