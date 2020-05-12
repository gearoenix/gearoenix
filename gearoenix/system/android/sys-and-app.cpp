#include "sys-and-app.hpp"

#if defined(GX_IN_ANDROID) && !defined(GX_USE_SDL)

#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-application.hpp"
#include "../../core/cr-static.hpp"
#include "../../core/event/cr-ev-engine.hpp"
#include "../../core/event/cr-ev-system.hpp"
#include "../../render/engine/rnd-eng-engine.hpp"
#include "../sys-log.hpp"

#ifdef GX_USE_OPENGL

#include "../../glc3/engine/glc3-eng-engine.hpp"
#include "../../gles2/engine/gles2-eng-engine.hpp"
#include "sys-gl-context.hpp"

#endif

#include <android_native_app_glue.h>
#include <string>

void gearoenix::system::Application::handle(android_app* const a, int32_t cmd) noexcept
{
    switch (cmd) {
    case APP_CMD_START:
        GXLOGD("Application started.")
        on_check_ready_to_render(a);
        break;
    case APP_CMD_RESUME:
        GXLOGD("Application resumed.")
        resumed = true;
        on_check_ready_to_render(a);
        break;
    case APP_CMD_PAUSE:
        GXLOGD("Application paused.")
        on_not_ready_to_render();
        break;
    case APP_CMD_STOP:
        GXLOGD("Application stopped.")
        on_not_ready_to_render();
        break;
    case APP_CMD_DESTROY:
        GXLOGD("Application destroyed.")
        break;
    case APP_CMD_GAINED_FOCUS:
        GXLOGD("Application focused.")
        focused = true;
        on_check_ready_to_render(a);
        break;
    case APP_CMD_LOST_FOCUS:
        GXLOGD("Application unfocused.")
        on_not_ready_to_render();
        break;
    case APP_CMD_INIT_WINDOW:
        GXLOGD("Application surface ready.")
        surface_ready = true;
        on_check_ready_to_render(a);
        break;
    case APP_CMD_TERM_WINDOW:
        GXLOGD("Android window terminated.")
        on_not_ready_to_render();
        break;
    case APP_CMD_WINDOW_RESIZED:
        GXLOGD("Android window resized.")
        // todo
        break;
    case APP_CMD_CONFIG_CHANGED:
        GXLOGD("Android config changed.")
        // todo
        break;
    default:
        GXLOGI("event not handled: " << cmd);
    }
}

int32_t gearoenix::system::Application::handle(android_app* const, AInputEvent* const e) noexcept
{
    const auto event_type = AInputEvent_getType(e);
    switch (event_type) {
    case AINPUT_EVENT_TYPE_KEY: {
        // TODO
        break;
    }
    case AINPUT_EVENT_TYPE_MOTION: {
        const auto action = AMotionEvent_getAction(e);
        const auto flags = action & AMOTION_EVENT_ACTION_MASK;
        const auto pointer_index = static_cast<size_t>((action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT);
        const auto pointer_id = AMotionEvent_getPointerId(e, pointer_index);
        const auto x = AMotionEvent_getRawX(e, pointer_index);
        const auto y = AMotionEvent_getRawY(e, pointer_index);
        switch (flags) {
        case AMOTION_EVENT_ACTION_DOWN:
        case AMOTION_EVENT_ACTION_POINTER_DOWN:
            event_engine->touch_down(
                static_cast<core::event::touch::FingerId>(pointer_id),
                static_cast<int>(x),
                event_engine->get_window_height() - static_cast<int>(y));
            break;
        case AMOTION_EVENT_ACTION_MOVE:
            event_engine->touch_move(
                static_cast<core::event::touch::FingerId>(pointer_id),
                static_cast<int>(x),
                event_engine->get_window_height() - static_cast<int>(y));
            break;
        case AMOTION_EVENT_ACTION_UP:
        case AMOTION_EVENT_ACTION_POINTER_UP:
            event_engine->touch_up(
                static_cast<core::event::touch::FingerId>(pointer_id),
                static_cast<int>(x),
                event_engine->get_window_height() - static_cast<int>(y));
            break;
        case AMOTION_EVENT_ACTION_CANCEL:
            event_engine->touch_cancel(
                static_cast<core::event::touch::FingerId>(pointer_id),
                static_cast<int>(x),
                event_engine->get_window_height() - static_cast<int>(y));
            break;
        default:
            GXLOGD("Unhandled motion event flag: " << flags)
            break;
        }
        break;
    }
    default:
        GXLOGE("Unexpected event type value: " << event_type)
        break;
    }
    return 0;
}

void gearoenix::system::Application::handle_cmd(android_app* const a, int32_t cmd) noexcept
{
    auto sys_app = static_cast<Application*>(a->userData);
    sys_app->handle(a, cmd);
}

int32_t gearoenix::system::Application::handle_input(android_app* const a, AInputEvent* const e) noexcept
{
    auto sys_app = static_cast<Application*>(a->userData);
    return sys_app->handle(a, e);
}

void gearoenix::system::Application::on_check_ready_to_render(android_app* const a) noexcept
{
    if (!resumed || !focused || !surface_ready)
        return;
    if (android_application->window == nullptr)
        return;
    if (render_engine == nullptr) {
#ifdef GX_USE_OPENGL
        if (gl_context != nullptr) {
            gl_context->init(android_application->window);
            event_engine->initialize_window_size(
                static_cast<std::size_t>(gl_context->get_screen_width()),
                static_cast<std::size_t>(gl_context->get_screen_height()));
        }
#endif
#ifdef GX_USE_OPENGL_ES3
        if (gl_context->get_es3_supported()) {
            GXLOGD("Going to create OpenGL ES3 render engine.")
            render_engine = std::unique_ptr<render::engine::Engine>(
                glc3::engine::Engine::construct(this, render::engine::Type::OPENGL_ES3));
        }
#endif
#ifdef GX_USE_OPENGL_ES2
        if (!gl_context->get_es3_supported()) {
            GXLOGD("Going to create OpenGL ES2 render engine.")
            render_engine = std::unique_ptr<render::engine::Engine>(
                gles2::engine::Engine::construct(this));
        }
#endif
        asset_manager = std::make_unique<core::asset::Manager>(this, GX_APP_DATA_NAME);
    } else if (a->window != android_application->window) {
        GXLOGF("Window reinitialized with different window (not implemented).")
        // todo
        // core::event::system::System eul(core::event::system::System::Action::UNLOAD);
        // core_app->on_event(eul);
        // render_engine->on_event(eul);
        // gl_ctx->Invalidate();
        // and_app = a;
        // gl_ctx->Init(a->window);
        // core::event::system::System erl(core::event::system::System::Action::RELOAD);
        // core_app->on_event(erl);
        // render_engine->on_event(erl);
    } else {
        GXLOGE("Window Reinitialized")
#ifdef GX_USE_OPENGL
        if (GX_RUNTIME_USE_OPENGL_E(render_engine)) {
            gl_context->resume(android_application->window);
            event_engine->initialize_window_size(
                static_cast<std::size_t>(gl_context->get_screen_width()),
                static_cast<std::size_t>(gl_context->get_screen_height()));
        }
#endif
        // todo: send event about reinitialization
    }
    running = true;
}

void gearoenix::system::Application::on_not_ready_to_render() noexcept
{
    if (!running)
        return;
    running = false;
    resumed = false;
    focused = false;
    surface_ready = false;
#ifdef GX_USE_OPENGL
    if (GX_RUNTIME_USE_OPENGL_E(render_engine)) {
        gl_context->suspend();
    }
#endif
}

gearoenix::system::Application::Application(android_app* and_app) noexcept
    : android_application(and_app)
    , event_engine(new core::event::Engine())
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
    core_application = nullptr;
    event_engine = nullptr;
    asset_manager = nullptr;
    render_engine = nullptr;
    gl_context = nullptr;
    android_application = nullptr;
}

void gearoenix::system::Application::execute(std::unique_ptr<core::Application> app) noexcept
{
    core_application = std::move(app);
    int events;
    android_poll_source* source;
    do {
        if (ALooper_pollAll(running ? 0 : -1, nullptr, &events,
                (void**)&source)
            >= 0) {
            if (source != nullptr)
                source->process(android_application.get(), source);
        }
        if (running) {
            core_application->update();
            render_engine->update();
#ifdef GX_USE_OPENGL
            if (GX_RUNTIME_USE_OPENGL_E(render_engine)) {
                gl_context->swap();
            }
#endif
        }
    } while (android_application->destroyRequested == 0);
    running = false;
    core_application->terminate();
    render_engine->terminate();
}

const char* gearoenix::system::Application::get_clipboard() const noexcept
{
    return nullptr;
}

#endif