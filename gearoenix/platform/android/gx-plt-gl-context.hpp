#ifndef GEAROENIX_PLATFORM_ANDROID_GL_CONTEXT_HPP
#define GEAROENIX_PLATFORM_ANDROID_GL_CONTEXT_HPP
#include "../gx-plt-build-configuration.hpp"
#ifdef GX_PLATFORM_INTERFACE_ANDROID
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include <EGL/egl.h>
#include <GLES2/gl2.h>

namespace gearoenix::platform {
struct GlContext final {
    GX_GET_VAL_PRV(int, screen_width, -1);
    GX_GET_VAL_PRV(int, screen_height, -1);
    GX_GET_VAL_PRV(int, depth_size, -1);
    GX_GET_VAL_PRV(int, samples_size, -1);

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
    void init_egl_display() noexcept;
    void init_egl_surface() noexcept;
    void init_egl_context() noexcept;

public:
    GlContext() noexcept = default;
    ~GlContext() noexcept;
    GlContext(GlContext const&) = delete;
    void operator=(GlContext const&) = delete;

    void init(ANativeWindow* window) noexcept;
    [[nodiscard]] State swap() noexcept;
    void invalidate() noexcept;
    void suspend() noexcept;
    void resume(ANativeWindow* window) noexcept;
};
}
#endif
#endif
#endif
