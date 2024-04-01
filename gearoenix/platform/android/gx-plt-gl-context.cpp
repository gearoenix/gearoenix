#include "gx-plt-gl-context.hpp"
#if defined(GX_PLATFORM_INTERFACE_ANDROID) && defined(GX_RENDER_OPENGL_ENABLED)
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../../opengl/gx-gl-loader.hpp"

void gearoenix::platform::GlContext::init_gles()
{
    if (gles_initialized)
        return;
    gles_initialized = true;
    if (gl::load_library()) {
        GX_LOG_D("OpenGL library loaded.");
        return;
    }
    GX_LOG_F("No suitable OpenGL library found");
}

void gearoenix::platform::GlContext::terminate()
{
    if (display != EGL_NO_DISPLAY) {
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (context != EGL_NO_CONTEXT) {
            eglDestroyContext(display, context);
        }

        if (surface != EGL_NO_SURFACE) {
            eglDestroySurface(display, surface);
        }
        eglTerminate(display);
    }
    display = EGL_NO_DISPLAY;
    context = EGL_NO_CONTEXT;
    surface = EGL_NO_SURFACE;
    context_valid = false;
}

bool gearoenix::platform::GlContext::check_surface(const EGLint opengl_version, const EGLint _depth_size, const EGLint _samples_size)
{
    const EGLint attribs[] = {
        EGL_RENDERABLE_TYPE, opengl_version,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_DEPTH_SIZE, _depth_size,
        EGL_SAMPLE_BUFFERS, _samples_size == 0 ? 0 : 1,
        EGL_SAMPLES, _samples_size,
        EGL_NONE
    };
    this->depth_size = static_cast<int>(_depth_size);
    this->samples_size = static_cast<int>(_samples_size);
    EGLint num_configs;
    return 0 != eglChooseConfig(display, attribs, &config, 1, &num_configs) && num_configs > 0 && config != nullptr;
}

void gearoenix::platform::GlContext::init_egl_display()
{
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    GX_ASSERT_D(EGL_NO_DISPLAY != display);
    eglInitialize(display, nullptr, nullptr);
}

void gearoenix::platform::GlContext::init_egl_surface()
{
    init_egl_display();
    constexpr EGLint configs[8][3] {
        { EGL_OPENGL_ES3_BIT, 32, 4 },
        { EGL_OPENGL_ES3_BIT, 24, 4 },
        { EGL_OPENGL_ES3_BIT, 32, 0 },
        { EGL_OPENGL_ES3_BIT, 24, 0 },
        { EGL_OPENGL_ES2_BIT, 32, 4 },
        { EGL_OPENGL_ES2_BIT, 24, 4 },
        { EGL_OPENGL_ES2_BIT, 32, 0 },
        { EGL_OPENGL_ES2_BIT, 24, 0 },
    };
    for (auto& c : configs) {
        if (check_surface(c[0], c[1], c[2])) {
            surface = eglCreateWindowSurface(display, config, window, nullptr);
            if (surface == nullptr)
                continue;
            eglQuerySurface(display, surface, EGL_WIDTH, &screen_width);
            eglQuerySurface(display, surface, EGL_HEIGHT, &screen_height);
            GX_LOG_D("Surface with OpenGL: " << (c[0] == EGL_OPENGL_ES3_BIT ? "ES3" : "ES2")
                                             << ", depth: " << c[1] << ", samples: " << c[2]);
            return;
        }
    }
    GX_LOG_F("No suitable surface found.");
}

void gearoenix::platform::GlContext::init_egl_context()
{
    context_valid = true;
    {
        GX_LOG_D("Trying to create OpenGL context.");
        const EGLint context_attribs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
        context = eglCreateContext(display, config, nullptr, context_attribs);
        if (context != nullptr && eglMakeCurrent(display, surface, surface, context) != EGL_FALSE)
            return;
    }
    GX_LOG_F("Can not create the required context");
}

gearoenix::platform::GlContext::~GlContext()
{
    terminate();
}

void gearoenix::platform::GlContext::init(ANativeWindow* const _window)
{
    if (egl_context_initialized)
        return;
    this->window = _window;
    init_egl_surface();
    init_egl_context();
    init_gles();
    egl_context_initialized = true;
}

gearoenix::platform::GlContext::State gearoenix::platform::GlContext::swap()
{
    const EGLBoolean b = eglSwapBuffers(display, surface);
    if (b == 0) {
        const EGLint err = eglGetError();
        if (err == EGL_BAD_SURFACE) {
            init_egl_surface();
            return State::RUNNING;
        } else if (err == EGL_CONTEXT_LOST || err == EGL_BAD_CONTEXT) {
            terminate();
            return State::TERMINATED;
        }
        GX_LOG_E("Unhandled error " << err);
    }
    return State::RUNNING;
}

void gearoenix::platform::GlContext::invalidate()
{
    terminate();
    egl_context_initialized = false;
}

void gearoenix::platform::GlContext::suspend()
{
    if (surface != EGL_NO_SURFACE) {
        eglDestroySurface(display, surface);
        surface = EGL_NO_SURFACE;
    }
}

void gearoenix::platform::GlContext::resume(ANativeWindow* const _window)
{
    if (!egl_context_initialized) {
        init(_window);
        return;
    }
    const int original_width = screen_width;
    const int original_height = screen_height;
    this->window = _window;
    surface = eglCreateWindowSurface(display, config, _window, nullptr);
    GX_ASSERT_D(EGL_NO_SURFACE != surface);
    eglQuerySurface(display, surface, EGL_WIDTH, &screen_width);
    eglQuerySurface(display, surface, EGL_HEIGHT, &screen_height);
    if (screen_width != original_width || screen_height != original_height)
        GX_LOG_D("Screen resized");
    if (eglMakeCurrent(display, surface, surface, context) == EGL_TRUE)
        return;
    const EGLint err = eglGetError();
    GX_LOG_D("Unable to eglMakeCurrent " << err);
    if (err == EGL_CONTEXT_LOST) {
        GX_LOG_D("Re-creating egl context");
        init_egl_context();
    } else {
        terminate();
        init_egl_surface();
        init_egl_context();
    }
}

#endif
