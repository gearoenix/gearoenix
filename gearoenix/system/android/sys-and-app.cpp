#include "sys-and-app.hpp"
#ifdef IN_ANDROID
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-application.hpp"
#include "../../core/event/cr-ev-bt-mouse.hpp"
#include "../../core/event/cr-ev-mv-mouse.hpp"
#include "../../core/event/cr-ev-sys-system.hpp"
#include "../../gles2/gles2-engine.hpp"
#include "../../render/rnd-engine.hpp"
#include "../sys-log.hpp"
#include <string>

void gearoenix::system::Application::handle_cmd(android_app* app, int32_t cmd)
{
    Application* sys_app = reinterpret_cast<Application*>(app->userData);
    sys_app->handle(app, cmd);
}

int32_t gearoenix::system::Application::handle_input(android_app* a, AInputEvent* e)
{
    Application* sys_app = reinterpret_cast<Application*>(a->userData);
    return sys_app->handle(a, e);
}

int32_t gearoenix::system::Application::handle(android_app* app, AInputEvent* e)
{
    ndk_helper::Vec2 p1, p2;
    core::Real w1, w2;
    ndk_helper::GESTURE_STATE pinch_state = pinch_detector.Detect(e);
    ndk_helper::GESTURE_STATE drag_state = drag_detector.Detect(e);
    ndk_helper::GESTURE_STATE tap_state = tap_detector.Detect(e);
    //    switch (pinch_state) {
    //        case ndk_helper::GESTURE_STATE_START: TODO;
    //            pinch_detector.GetPointers(p1, p2);
    //            w = (p1 - p2).Length();
    //            return 1;
    //        case ndk_helper::GESTURE_STATE_MOVE:
    //            pinch_detector.GetPointers(p1, p2);
    //            w2 = (p1 - p2).Length();
    ////        core_app->on_scroll((w2 - w) * 0.03f);
    //            w = w2;
    //            return 1;
    //        default:
    //            break;
    //    }
    //    switch (drag_state) {
    //        case ndk_helper::GESTURE_STATE_START: {
    //            TODO;
    //            drag_detector.GetPointer(p1);
    //            p1.Value(x, y);
    //            x = convert_pixel_x_to_normalized((int) x);
    //            y = convert_pixel_y_to_normalized((int) y);
    //            core::event::button::Mouse me(
    //                    core::event::button::Button::LEFT,
    //                    core::event::button::Button::PRESS,
    //                    x, y
    //            );
    //            render_engine->on_event(me);
    //            core_app->on_event(me);
    //            return 1;
    //        }
    //        case ndk_helper::GESTURE_STATE_MOVE: {
    //            drag_detector.GetPointer(p1);
    //            p1.Value(w1, w2);
    //            w1 = convert_pixel_x_to_normalized((int) w1);
    //            w2 = convert_pixel_y_to_normalized((int) w2);
    //            core::event::movement::Mouse me(w1, w2, x, y);
    //            render_engine->on_event(me);
    //            core_app->on_event(me);
    //            x = w1;
    //            y = w2;
    //            return 1;
    //        }
    //        case ndk_helper::GESTURE_STATE_END: {
    //            drag_detector.GetPointer(p1);
    //            p1.Value(x, y);
    //            x = convert_pixel_x_to_normalized((int) x);
    //            y = convert_pixel_y_to_normalized((int) y);
    //            core::event::button::Mouse me(
    //                    core::event::button::Button::LEFT,
    //                    core::event::button::Button::RELEASE,
    //                    x, y
    //            );
    //            render_engine->on_event(me);
    //            core_app->on_event(me);
    //            return 1;
    //        }
    //        default:
    //            break;
    //    }
    switch (tap_state) {
    case ndk_helper::GESTURE_STATE_ACTION: {
        x = convert_pixel_x_to_normalized((int)AMotionEvent_getX(e, 0));
        y = convert_pixel_y_to_normalized((int)AMotionEvent_getY(e, 0));
        core::event::button::Mouse me(
            core::event::button::Button::LEFT,
            core::event::button::Button::PRESS,
            x, y);
        render_engine->on_event(me);
        core_app->on_event(me);
        core::event::button::Mouse me1(
            core::event::button::Button::LEFT,
            core::event::button::Button::RELEASE,
            x, y);
        render_engine->on_event(me1);
        core_app->on_event(me1);
        return 1;
    }
    default:
        break;
    }
    return 0;
}

gearoenix::system::Application::Application(android_app* and_app)
    : and_app(and_app)
    , gl_ctx(ndk_helper::GLContext::GetInstance())
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

gearoenix::system::Application::~Application()
{
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
    return screen_ratio;
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
        if (ALooper_pollAll(active ? 0 : -1, nullptr, &events,
                (void**)&source)
            >= 0) {
            if (source != nullptr)
                source->process(and_app, source);
        }
        if (active) {
            core_app->update();
            render_engine->update();
            if (gl_ctx->Swap() != EGL_SUCCESS) {
                GXLOGE("reached");
                core::event::system::System eul(core::event::system::System::Action::UNLOAD);
                core_app->on_event(eul);
                render_engine->on_event(eul);
                core::event::system::System erl(core::event::system::System::Action::RELOAD);
                core_app->on_event(erl);
                render_engine->on_event(erl);
                GXLOGE("reached");
            }
        }
    } while (and_app->destroyRequested == 0);
    active = false;
    core_app->terminate();
    render_engine->terminate();
    delete core_app;
    core_app = nullptr;
    delete render_engine;
    render_engine = nullptr;
    delete astmgr;
    astmgr = nullptr;
    gl_ctx->Invalidate();
    gl_ctx = nullptr;
    delete and_app;
    and_app = nullptr;
    return;
}

void gearoenix::system::Application::handle(android_app* a, int32_t cmd)
{
    switch (cmd) {
    case APP_CMD_INIT_WINDOW:
        if (and_app->window != nullptr) {
            if (render_engine == nullptr) {
                gl_ctx->Init(and_app->window);
                win_width = (unsigned int)gl_ctx->GetScreenWidth();
                win_height = (unsigned int)gl_ctx->GetScreenHeight();
                screen_ratio = (core::Real)win_width / (core::Real)win_height;
                half_height_inversed = 2.0f / (core::Real)win_height;
                astmgr = new core::asset::Manager(this, "data.gx3d");
                astmgr->initialize();
                render_engine = new gles2::Engine(this);
                astmgr->set_render_engine(render_engine);
            } else if (a->window != and_app->window) {
                GXLOGE("reached");
                core::event::system::System eul(core::event::system::System::Action::UNLOAD);
                core_app->on_event(eul);
                render_engine->on_event(eul);
                gl_ctx->Invalidate();
                and_app = a;
                gl_ctx->Init(a->window);
                core::event::system::System erl(core::event::system::System::Action::RELOAD);
                core_app->on_event(erl);
                render_engine->on_event(erl);
                GXLOGE("reached");
            } else if (EGL_SUCCESS == gl_ctx->Resume(a->window)) {
                GXLOGE("reached");
                core::event::system::System eul(core::event::system::System::Action::UNLOAD);
                core_app->on_event(eul);
                render_engine->on_event(eul);
                core::event::system::System erl(core::event::system::System::Action::RELOAD);
                core_app->on_event(erl);
                render_engine->on_event(erl);
                GXLOGE("reached");
            } else
                UNEXPECTED;
            win_width = (unsigned int)gl_ctx->GetScreenWidth();
            win_height = (unsigned int)gl_ctx->GetScreenHeight();
            screen_ratio = (core::Real)win_width / (core::Real)win_height;
            half_height_inversed = 2.0f / (core::Real)win_height;
            active = true;
        }
        break;
    case APP_CMD_TERM_WINDOW: {
        if (core_app == nullptr || render_engine == nullptr)
            break;
        active = false;
        core::event::system::System eul(core::event::system::System::Action::UNLOAD);
        core_app->on_event(eul);
        render_engine->on_event(eul);
        gl_ctx->Suspend();
        break;
    }
    default:
        GXLOGI("event not handled: " << cmd);
    }
}

gearoenix::core::Real gearoenix::system::Application::convert_pixel_x_to_normalized(int x)
{
    return ((((core::Real)x) * half_height_inversed) - screen_ratio);
}

gearoenix::core::Real gearoenix::system::Application::convert_pixel_y_to_normalized(int y)
{
    return (1.0f - (((core::Real)y) * half_height_inversed));
}

#endif
