#include "sys-and-app.hpp"
#ifdef IN_ANDROID
#include "../../core/cr-application.hpp"
#include "../../render/rnd-engine.hpp"
#include "../sys-file.hpp"
#include "../sys-log.hpp"
#include <android_native_app_glue.h>
#include <string>

void gearoenix::system::Application::handle_cmd(android_app* app, int32_t cmd)
{
    Application* sys_app = reinterpret_cast<Application*>(app->userData);
    sys_app->handle(cmd);
}

gearoenix::system::Application::Application(android_app* and_app)
    : and_app(and_app)
{
    and_app->userData = this;
    and_app->onAppCmd = gearoenix::system::Application::handle_cmd;
    int events;
    android_poll_source* source;
    do {
        if (ALooper_pollAll(1, nullptr, &events, (void**)&source) >= 0) {
            if (source != nullptr)
                source->process(and_app, source);
        }
    } while (and_app->destroyRequested == 0);
}

gearoenix::system::Application::~Application()
{
    delete core_app;
    core_app = nullptr;
    delete render_engine;
    render_engine = nullptr;
}

android_app* gearoenix::system::Application::get_android_app() const
{
    return and_app;
}

const gearoenix::core::Application* gearoenix::system::Application::get_core_app() const
{
    return core_app;
}

gearoenix::core::Application* gearoenix::system::Application::get_core_app()
{
    return core_app;
}

const gearoenix::render::Engine* gearoenix::system::Application::get_render_engine() const
{
    return render_engine;
}

gearoenix::render::Engine* gearoenix::system::Application::get_render_engine()
{
    return render_engine;
}

const gearoenix::core::asset::Manager* gearoenix::system::Application::get_asset_manager() const
{
    return astmgr;
}

gearoenix::core::asset::Manager* gearoenix::system::Application::get_asset_manager()
{
    return astmgr;
}

gearoenix::core::Real gearoenix::system::Application::get_window_ratio() const
{
    return ((gearoenix::core::Real)win_width) / ((gearoenix::core::Real)win_height);
}

unsigned int gearoenix::system::Application::get_width() const
{
    return win_width;
}

unsigned int gearoenix::system::Application::get_height() const
{
    return win_height;
}

void gearoenix::system::Application::execute(core::Application* app)
{
    core_app = app;
    int events;
    android_poll_source* source;
    do {
        if (ALooper_pollAll(0, nullptr, &events,
                (void**)&source)
            >= 0) {
            if (source != nullptr)
                source->process(and_app, source);
        }
    } while (and_app->destroyRequested == 0);
}

void gearoenix::system::Application::handle(int32_t cmd)
{
    switch (cmd) {
    case APP_CMD_INIT_WINDOW:
        if (and_app->window != nullptr) {
            const EGLint attribs[] = {
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_BLUE_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_RED_SIZE, 8,
                EGL_NONE
            };
            EGLint w, h, format;
            EGLint num_configs;
            EGLConfig config;
            display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
            eglInitialize(display, 0, 0);
            eglChooseConfig(display, attribs, nullptr, 0, &num_configs);
            std::unique_ptr<EGLConfig[]> supported_configs(new EGLConfig[num_configs]);
            eglChooseConfig(display, attribs, supported_configs.get(), num_configs, &num_configs);
            if (num_configs == 0) {
                UNEXPECTED;
            }
            int i = 0;
            for (; i < num_configs; i++) {
                EGLConfig& cfg = supported_configs[i];
                EGLint r, g, b, d;
                if (eglGetConfigAttrib(display, cfg, EGL_RED_SIZE, &r) && eglGetConfigAttrib(display, cfg, EGL_GREEN_SIZE, &g) && eglGetConfigAttrib(display, cfg, EGL_BLUE_SIZE, &b) && eglGetConfigAttrib(display, cfg, EGL_DEPTH_SIZE, &d) && r == 8 && g == 8 && b == 8 && d == 24) {

                    config = supported_configs[i];
                    break;
                }
            }
            if (i == num_configs) {
                config = supported_configs[0];
            }
            eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
            surface = eglCreateWindowSurface(display, config, this->and_app->window, NULL);
            context = eglCreateContext(display, config, NULL, NULL);
            if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
                LOGF("Unable to eglMakeCurrent");
            }
            eglQuerySurface(display, surface, EGL_WIDTH, &w);
            eglQuerySurface(display, surface, EGL_HEIGHT, &h);
            win_width = (unsigned int)w;
            win_height = (unsigned int)h;
        }
        break;
    case APP_CMD_LOST_FOCUS:
        if (core_app != nullptr)
            core_app->terminate();
        if (render_engine != nullptr)
            render_engine->terminate();
        break;
    default:
        LOGI("event not handled: " << cmd);
    }
}

#endif
