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

void gearoenix::system::Application::handle(android_app* a, int32_t cmd) noexcept
{
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            if (and_app->window != nullptr) {
                if (render_engine == nullptr) {
                    // TODO: If Vulkan was supported don't go any further
                gl_context->init(and_app->window);
                if(gl_context->get_es3_supported()) {
                    render_engine = glc3::engine::Engine::construct(this, render::engine::Type::OPENGL_ES3);
                } else {
                    render_engine = gles2::engine::Engine::construct(this);
                }
//                win_width = (unsigned int)gl_ctx->GetScreenWidth();
//                win_height = (unsigned int)gl_ctx->GetScreenHeight();
//                screen_ratio = (core::Real)win_width / (core::Real)win_height;
//                half_height_inversed = 2.0f / (core::Real)win_height;
//                astmgr = new core::asset::Manager(this, "data.gx3d");
//                astmgr->initialize();
//                render_engine = new gles2::Engine(this);
//                astmgr->set_render_engine(render_engine);
                } else if (a->window != and_app->window) {
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
                } else
                GXUNEXPECTED;
//            win_width = (unsigned int)gl_ctx->GetScreenWidth();
//            win_height = (unsigned int)gl_ctx->GetScreenHeight();
                screen_ratio = (core::Real)win_width / (core::Real)win_height;
                half_height_inverted = 2.0F / (core::Real)win_height;
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
        default:
        GXLOGI("event not handled: " << cmd);
    }
}

int32_t gearoenix::system::Application::handle(android_app*const, AInputEvent*const e) noexcept
{
//    core::event::Event* gxe = nullptr;
    const int32_t action = AMotionEvent_getAction(e);
    const auto flags = static_cast<int32_t>(static_cast<unsigned int>(action) & AMOTION_EVENT_ACTION_MASK);
    const core::Real curx = convert_x_to_ratio((int)AMotionEvent_getX(e, 0));
    const core::Real cury = convert_y_to_ratio((int)AMotionEvent_getY(e, 0));
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

void gearoenix::system::Application::handle_cmd(android_app* app, int32_t cmd) noexcept
{
    auto sys_app = static_cast<Application*>(app->userData);
    sys_app->handle(app, cmd);
}

int32_t gearoenix::system::Application::handle_input(android_app* a, AInputEvent* e) noexcept
{
    auto sys_app = static_cast<Application*>(a->userData);
    return sys_app->handle(a, e);
}

gearoenix::system::Application::Application(android_app* and_app) noexcept
    : and_app(and_app)
    , gl_context(new GlContext())
{
    and_app->userData = this;
    and_app->onAppCmd = gearoenix::system::Application::handle_cmd;
    and_app->onInputEvent = gearoenix::system::Application::handle_input;
    int events;
    android_poll_source* source;
    do {
        if (ALooper_pollAll(-1, nullptr, &events, (void**)&source) >= 0) {
            if (source != nullptr)
                source->process(and_app, source);
        }
        if (render_engine != nullptr)
            break;
    } while (and_app->destroyRequested == 0);
}

gearoenix::system::Application::~Application() noexcept
{
    GX_DELETE(core_app)
    GX_DELETE(event_engine)
    GX_DELETE(render_engine)
    GX_DELETE(astmgr)
    GX_DELETE(gl_context)
    GX_DELETE(and_app)
}

android_app* gearoenix::system::Application::get_android_app() const noexcept
{
    return and_app;
}

const gearoenix::core::Application* gearoenix::system::Application::get_core_app() const noexcept
{
    return core_app;
}

gearoenix::core::Application* gearoenix::system::Application::get_core_app() noexcept
{
    return core_app;
}

const gearoenix::render::engine::Engine* gearoenix::system::Application::get_render_engine() const noexcept
{
    return render_engine;
}

gearoenix::render::engine::Engine* gearoenix::system::Application::get_render_engine() noexcept
{
    return render_engine;
}

const gearoenix::core::asset::Manager* gearoenix::system::Application::get_asset_manager() const noexcept
{
    return astmgr;
}

gearoenix::core::asset::Manager* gearoenix::system::Application::get_asset_manager() noexcept
{
    return astmgr;
}

gearoenix::core::Real gearoenix::system::Application::get_window_ratio() const noexcept
{
    return screen_ratio;
}

unsigned int gearoenix::system::Application::get_width() const noexcept
{
    return win_width;
}

unsigned int gearoenix::system::Application::get_height() const noexcept
{
    return win_height;
}

void gearoenix::system::Application::execute(core::Application* app) noexcept
{
    core_app = app;
    int events;
    android_poll_source* source;
    do {
        if (ALooper_pollAll(running ? 0 : -1, nullptr, &events,
                (void**)&source)
            >= 0) {
            if (source != nullptr)
                source->process(and_app, source);
        }
        if (running) {
            core_app->update();
            render_engine->update();
//            if (gl_ctx->Swap() != EGL_SUCCESS) {
//                GXLOGE("reached");
//                core::event::system::System eul(core::event::system::System::Action::UNLOAD);
//                core_app->on_event(eul);
//                render_engine->on_event(eul);
//                core::event::system::System erl(core::event::system::System::Action::RELOAD);
//                core_app->on_event(erl);
//                render_engine->on_event(erl);
//                GXLOGE("reached");
//            }
        }
    } while (and_app->destroyRequested == 0);
    running = false;
    core_app->terminate();
    render_engine->terminate();
}

gearoenix::core::Real gearoenix::system::Application::convert_x_to_ratio(const int x) const noexcept
{
    return ((((core::Real)x) * half_height_inverted) - screen_ratio);
}

gearoenix::core::Real gearoenix::system::Application::convert_y_to_ratio(const int y) const noexcept
{
    return (1.0F - (((core::Real)y) * half_height_inverted));
}
#endif