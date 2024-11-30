#pragma once
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

    void init_gles();
    void terminate();
    bool check_surface(EGLint opengl_version, EGLint depth_size, EGLint samples_size);
    void init_egl_display();
    void init_egl_surface();
    void init_egl_context();

public:
    GlContext() = default;
    ~GlContext();
    GlContext(GlContext const&) = delete;
    void operator=(GlContext const&) = delete;

    void init(ANativeWindow* window);
    [[nodiscard]] State swap();
    void invalidate();
    void suspend();
    void resume(ANativeWindow* window);
};
}
#endif
#endif