#include "gx-sys-gl-context.hpp"
#if defined(GX_IN_ANDROID) && !defined(GX_USE_SDL) && defined(GX_USE_OPENGL)
#include "../../gl/gx-gl-loader.hpp"
#include "../gx-sys-log.hpp"

void gearoenix::system::GlContext::init_gles() noexcept
{
    if (gles_initialized)
        return;
    gles_initialized = true;
#ifdef GX_USE_OPENGL_ES3
    if (gl::Loader::load_library(render::engine::Type::OpenGLES3)) {
        GXLOGD("OpenGL ES3 library loaded.")
        es3_supported = true;
        return;
    }
#endif
#ifdef GX_USE_OPENGL_ES2
    if (gl::Loader::load_library(render::engine::Type::OpenGLES2)) {
        GXLOGD("OpenGL ES2 library loaded.")
        return;
    }
#endif
    GXLOGF("No suitable OpenGL library found")
}

void gearoenix::system::GlContext::terminate() noexcept
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

bool gearoenix::system::GlContext::check_surface(const EGLint opengl_version, const EGLint depth_size, const EGLint samples_size) noexcept
{
    const EGLint attribs[] = {
        EGL_RENDERABLE_TYPE, opengl_version,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_DEPTH_SIZE, depth_size,
        EGL_SAMPLE_BUFFERS, samples_size == 0 ? 0 : 1,
        EGL_SAMPLES, samples_size,
        EGL_NONE
    };
    this->depth_size = static_cast<int>(depth_size);
    this->samples_size = static_cast<int>(samples_size);
    EGLint num_configs;
    return 0 != eglChooseConfig(display, attribs, &config, 1, &num_configs) && num_configs > 0 && config != nullptr;
}

void gearoenix::system::GlContext::init_egl_surface() noexcept
{
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(display, nullptr, nullptr);
#if defined(GX_USE_OPENGL_ES3) && defined(GX_USE_OPENGL_ES2)
#define GX_EGL_CONFIGS_COUNT 8
#else
#define GX_EGL_CONFIGS_COUNT 4
#endif
    constexpr EGLint configs[GX_EGL_CONFIGS_COUNT][3] = {
#ifdef GX_USE_OPENGL_ES3
        { EGL_OPENGL_ES3_BIT, 32, 4 },
        { EGL_OPENGL_ES3_BIT, 24, 4 },
        { EGL_OPENGL_ES3_BIT, 32, 0 },
        { EGL_OPENGL_ES3_BIT, 24, 0 },
#endif
#ifdef GX_USE_OPENGL_ES2
        { EGL_OPENGL_ES2_BIT, 32, 4 },
        { EGL_OPENGL_ES2_BIT, 24, 4 },
        { EGL_OPENGL_ES2_BIT, 32, 0 },
        { EGL_OPENGL_ES2_BIT, 24, 0 },
#endif
    };
    for (auto& c : configs)
        if (check_surface(c[0], c[1], c[2])) {
            surface = eglCreateWindowSurface(display, config, window, nullptr);
            if (surface == nullptr)
                continue;
            eglQuerySurface(display, surface, EGL_WIDTH, &screen_width);
            eglQuerySurface(display, surface, EGL_HEIGHT, &screen_height);
            GXLOGD("Surface with OpenGL: " << (c[0] == EGL_OPENGL_ES3_BIT ? "ES3" : "ES2") << ", depth: " << c[1] << ", samples: " << c[2])
            return;
        }
    GXLOGF("No suitable surface found.")
}

void gearoenix::system::GlContext::init_egl_context() noexcept
{
    context_valid = true;
#ifdef GX_USE_OPENGL_ES3
    {
        GXLOGD("Trying to create OpenGL context 3")
        const EGLint context_attribs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
        context = eglCreateContext(display, config, nullptr, context_attribs);
        if (context != nullptr && eglMakeCurrent(display, surface, surface, context) != EGL_FALSE)
            return;
    }
#endif
#ifdef GX_USE_OPENGL_ES2
    {
        GXLOGD("Trying to create OpenGL context 2")
        const EGLint context_attribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
        context = eglCreateContext(display, config, nullptr, context_attribs);
        if (context != nullptr && eglMakeCurrent(display, surface, surface, context) != EGL_FALSE)
            return;
    }
#endif
    GXLOGF("Can not create the required context")
}

gearoenix::system::GlContext::~GlContext() noexcept
{
    terminate();
}

void gearoenix::system::GlContext::init(ANativeWindow* const window) noexcept
{
    if (egl_context_initialized)
        return;
    this->window = window;
    init_egl_surface();
    init_egl_context();
    init_gles();
    egl_context_initialized = true;
}

gearoenix::system::GlContext::State gearoenix::system::GlContext::swap() noexcept
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
        GXLOGE("Unhandled error " << err)
    }
    return State::RUNNING;
}

void gearoenix::system::GlContext::invalidate() noexcept
{
    terminate();
    egl_context_initialized = false;
}

void gearoenix::system::GlContext::suspend() noexcept
{
    if (surface != EGL_NO_SURFACE) {
        eglDestroySurface(display, surface);
        surface = EGL_NO_SURFACE;
    }
}

void gearoenix::system::GlContext::resume(ANativeWindow* const window) noexcept
{
    if (!egl_context_initialized) {
        init(window);
        return;
    }
    const int original_width = screen_width;
    const int original_height = screen_height;
    this->window = window;
    surface = eglCreateWindowSurface(display, config, window, nullptr);
    eglQuerySurface(display, surface, EGL_WIDTH, &screen_width);
    eglQuerySurface(display, surface, EGL_HEIGHT, &screen_height);
    if (screen_width != original_width || screen_height != original_height)
        GXLOGD("Screen resized");
    if (eglMakeCurrent(display, surface, surface, context) == EGL_TRUE)
        return;
    const EGLint err = eglGetError();
    GXLOGD("Unable to eglMakeCurrent " << err);
    if (err == EGL_CONTEXT_LOST) {
        GXLOGD("Re-creating egl context");
        init_egl_context();
    } else {
        terminate();
        init_egl_surface();
        init_egl_context();
    }
}
#endif
