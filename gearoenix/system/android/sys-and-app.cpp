#include "sys-and-app.hpp"

#if defined(GX_IN_ANDROID) && !defined(GX_USE_SDL)

#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-application.hpp"
#include "../../core/cr-static.hpp"
#include "../../core/event/cr-ev-sys-system.hpp"
#include "../../core/event/cr-ev-engine.hpp"
#include "../../render/engine/rnd-eng-engine.hpp"
#include "../sys-log.hpp"

#ifdef GX_USE_OPENGL

#include "../../glc3/engine/glc3-eng-engine.hpp"
#include "../../gles2/engine/gles2-eng-engine.hpp"
#include "sys-gl-context.hpp"

#endif

#include <android_native_app_glue.h>
#include <string>

void gearoenix::system::Application::handle(android_app *a, int32_t cmd) noexcept {
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            if (android_application->window != nullptr) {
                if (render_engine == nullptr) {
                    // TODO: If Vulkan was supported don't go any further
                    gl_context->init(android_application->window);
                    window_width = static_cast<unsigned int>(gl_context->get_screen_width());
                    window_height = static_cast<unsigned int>(gl_context->get_screen_height());
                    compute_screen_ratios();
                    if (gl_context->get_es3_supported()) {
                        GXLOGD("Going to create OpenGL ES3 render engine.")
                        render_engine = glc3::engine::Engine::construct(this,
                                                                        render::engine::Type::OPENGL_ES3);
                    } else {
                        GXLOGD("Going to create OpenGL ES2 render engine.")
                        render_engine = gles2::engine::Engine::construct(this);
                    }
                    asset_manager = new core::asset::Manager(this, GX_APP_DATA_NAME);
                } else if (a->window != android_application->window) {
//                GXLOGE("reached");
//                core::event::system::System eul(core::event::system::System::Action::UNLOAD);
//                core_app->on_event(eul);
//                render_engine->on_event(eul);
//                gl_ctx->Invalidate();
//                and_app = a;
//                gl_ctx->Init(a->window);
//                core::event::system::System erl(core::event::system::System::Action::RELOAD);
//                core_app->on_event(erl);
//                render_engine->on_event(erl);
//                GXLOGE("reached");
//            } else if (EGL_SUCCESS == gl_ctx->Resume(a->window)) {
//                GXLOGE("reached");
//                core::event::system::System eul(core::event::system::System::Action::UNLOAD);
//                core_app->on_event(eul);
//                render_engine->on_event(eul);
//                core::event::system::System erl(core::event::system::System::Action::RELOAD);
//                core_app->on_event(erl);
//                render_engine->on_event(erl);
//                GXLOGE("reached");
                } else GXUNEXPECTED;
//            win_width = (unsigned int)gl_ctx->GetScreenWidth();
//            win_height = (unsigned int)gl_ctx->GetScreenHeight();
                running = true;
            }
            break;
        case APP_CMD_TERM_WINDOW: {
//        if (core_app == nullptr || render_engine == nullptr)
//            break;
//        active = false;
//        core::event::system::System eul(core::event::system::System::Action::UNLOAD);
//        core_app->on_event(eul);
//        render_engine->on_event(eul);
//        gl_ctx->Suspend();
//        break;
        }
        default: GXLOGI("event not handled: " << cmd);
    }
}

int32_t gearoenix::system::Application::handle(android_app *const, AInputEvent *const e) noexcept {
//    core::event::Event* gxe = nullptr;
    const int32_t action = AMotionEvent_getAction(e);
    const auto flags = static_cast<int32_t>(static_cast<unsigned int>(action) &
                                            AMOTION_EVENT_ACTION_MASK);
    const core::Real curx = convert_x_to_ratio((int) AMotionEvent_getX(e, 0));
    const core::Real cury = convert_y_to_ratio((int) AMotionEvent_getY(e, 0));
    switch (flags) {
        case AMOTION_EVENT_ACTION_DOWN:
            // TODO
            break;
        case AMOTION_EVENT_ACTION_MOVE:
            // TODO
            break;
        case AMOTION_EVENT_ACTION_UP:
            // TODO
            break;
        default:
            break;
    }
    x = curx;
    y = cury;
    return 0;
}

void gearoenix::system::Application::handle_cmd(android_app *app, int32_t cmd) noexcept {
    auto sys_app = static_cast<Application *>(app->userData);
    sys_app->handle(app, cmd);
}

int32_t gearoenix::system::Application::handle_input(android_app *a, AInputEvent *e) noexcept {
    auto sys_app = static_cast<Application *>(a->userData);
    return sys_app->handle(a, e);
}

void gearoenix::system::Application::compute_screen_ratios() noexcept {
    constexpr core::Real DOUBLE_F = 2.0F;
    window_ratio = static_cast<core::Real>(window_width) / static_cast<core::Real>(window_height);
    half_height_inverted = DOUBLE_F / static_cast<core::Real>(window_height);
}

gearoenix::system::Application::Application(android_app *and_app) noexcept
        : android_application(and_app), gl_context(new GlContext()), event_engine(new core::event::Engine()) {
    and_app->userData = this;
    and_app->onAppCmd = gearoenix::system::Application::handle_cmd;
    and_app->onInputEvent = gearoenix::system::Application::handle_input;
    int events;
    android_poll_source *source;
    do {
        if (ALooper_pollAll(-1, nullptr, &events, (void **) &source) >= 0) {
            if (source != nullptr)
                source->process(and_app, source);
        }
        if (render_engine != nullptr)
            break;
    } while (and_app->destroyRequested == 0);
}

gearoenix::system::Application::~Application() noexcept {
    GX_DELETE(core_app)
    GX_DELETE(event_engine)
    GX_DELETE(asset_manager)
    GX_DELETE(render_engine)
    GX_DELETE(gl_context)
    GX_DELETE(android_application)
}

void gearoenix::system::Application::execute(core::Application *app) noexcept {
    core_app = app;
    int events;
    android_poll_source *source;
    do {
        if (ALooper_pollAll(running ? 0 : -1, nullptr, &events,
                            (void **) &source)
            >= 0) {
            if (source != nullptr)
                source->process(android_application, source);
        }
        if (running) {
            core_app->update();
            render_engine->update();
#ifdef GX_USE_OPENGL
            if (GX_RUNTIME_USE_OPENGL_E(render_engine)) {
                gl_context->swap();
            }
#endif
        }
    } while (android_application->destroyRequested == 0);
    running = false;
    core_app->terminate();
    render_engine->terminate();
}

gearoenix::core::Real
gearoenix::system::Application::convert_x_to_ratio(const int x) const noexcept {
    return ((((core::Real) x) * half_height_inverted) - window_ratio);
}

gearoenix::core::Real
gearoenix::system::Application::convert_y_to_ratio(const int y) const noexcept {
    return (1.0F - (((core::Real) y) * half_height_inverted));
}

#endif