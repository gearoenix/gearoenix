#ifndef GEAROENIX_SYSTEM_ANDROID_GL_CONTEXT_HPP
#define GEAROENIX_SYSTEM_ANDROID_GL_CONTEXT_HPP
#include "../../core/cr-build-configuration.hpp"
#if defined(GX_IN_ANDROID) && !defined(GX_USE_SDL) && defined(GX_USE_OPENGL)
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include "../../core/cr-static.hpp"

namespace gearoenix::system {
    class GlContext {
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

        int screen_width = -1;
        int screen_height = -1;
        int depth_size = -1;
        int samples_size = -1;

        bool gles_initialized = false;
        bool egl_context_initialized = false;
        bool es3_supported = false;
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

        GX_GETTER_BUILDER_V(screen_width)
        GX_GETTER_BUILDER_V(screen_height)
        GX_GETTER_BUILDER_V(depth_size)
        GX_GETTER_BUILDER_V(samples_size)
        GX_GETTER_BUILDER_V(es3_supported)

        bool check_extension( const char* extension ) noexcept;
    };
}
#endif
#endif
