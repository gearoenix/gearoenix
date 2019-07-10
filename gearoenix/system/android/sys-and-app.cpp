#include "sys-and-app.hpp"
#if defined(GX_IN_ANDROID) && !defined(GX_USE_SDL)
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
    Application* sys_app = static_cast<Application*>(app->userData);
    sys_app->handle(app, cmd);
}

int32_t gearoenix::system::Application::handle_input(android_app* a, AInputEvent* e)
{
    Application* sys_app = static_cast<Application*>(a->userData);
    return sys_app->handle(a, e);
}

int32_t gearoenix::system::Application::handle(android_app* app, AInputEvent* e)
{
    core::event::Event* gxe = nullptr;
    int32_t action = AMotionEvent_getAction(e);
    int32_t flags = action & AMOTION_EVENT_ACTION_MASK;
    const core::Real curx = convert_pixel_x_to_normalized((int)AMotionEvent_getX(e, 0));
    const core::Real cury = convert_pixel_y_to_normalized((int)AMotionEvent_getY(e, 0));
    switch (flags) {
    case AMOTION_EVENT_ACTION_DOWN:
        gxe = new core::event::button::Mouse(
            core::event::button::Button::LEFT,
            core::event::button::Button::PRESS,
            curx, cury);
        break;
    case AMOTION_EVENT_ACTION_MOVE:
        gxe = new core::event::movement::Mouse(
            curx, cury, x, y);
        break;
    case AMOTION_EVENT_ACTION_UP:
        gxe = new core::event::button::Mouse(
            core::event::button::Button::LEFT,
            core::event::button::Button::RELEASE,
            curx, cury);
        break;
    default:
        break;
    }
    x = curx;
    y = cury;
    if (nullptr != gxe) {
        render_engine->on_event(*gxe);
        core_app->on_event(*gxe);
        return 1;
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