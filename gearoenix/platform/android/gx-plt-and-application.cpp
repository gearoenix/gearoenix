#include "gx-plt-and-application.hpp"

#if defined(GX_IN_ANDROID) && !defined(GX_USE_SDL)

#include "../../core/asset/gx-cr-asset-manager.hpp"
#include "../../core/event/gx-cr-ev-engine.hpp"
#include "../../core/event/gx-cr-ev-system.hpp"
#include "../../core/gx-cr-application.hpp"
#include "../../core/gx-cr-static.hpp"
#include "../../render/engine/gx-rnd-eng-engine.hpp"
#include "../gx-plt-log.hpp"

#ifdef GX_USE_OPENGL

#include "../../glc3/engine/gx-glc3-eng-engine.hpp"
#include "../../gles2/engine/gx-gles2-eng-engine.hpp"
#include "gx-plt-gl-context.hpp"

#endif

#include <android_native_app_glue.h>
#include <string>

void gearoenix::platform::Application::handle(android_app* const a, int32_t cmd) noexcept
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
        resumed = false;
        on_not_ready_to_render();
        break;
    case APP_CMD_STOP:
        GXLOGD("Application stopped.")
        resumed = false;
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
        focused = false;
        on_not_ready_to_render();
        break;
    case APP_CMD_INIT_WINDOW:
        GXLOGD("Application surface ready.")
        surface_ready = true;
        on_check_ready_to_render(a);
        break;
    case APP_CMD_TERM_WINDOW:
        GXLOGD("Android window terminated.")
        surface_ready = false;
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

int32_t gearoenix::platform::Application::handle(android_app* const, AInputEvent* const e) noexcept
{
    const auto event_type = AInputEvent_getType(e);
    switch (event_type) {
    case AINPUT_EVENT_TYPE_KEY: {
        event_engine->broadcast(core::event::Data(core::event::Id::ButtonKeyboard, core::event::button::KeyboardData([&]() noexcept {
            switch (AKeyEvent_getAction(e)) {
                case AKEY_EVENT_ACTION_UP:
                    return core::event::button::KeyboardActionId::Release;
                case AKEY_EVENT_ACTION_DOWN:
                    return core::event::button::KeyboardActionId::Press;
                default:
                    return core::event::button::KeyboardActionId::Unknown;
            } }(), [&]() noexcept {
            const auto key = AKeyEvent_getKeyCode(e);
            switch (key) {
                case AKEYCODE_DPAD_LEFT:
                case AKEYCODE_META_LEFT:
                case AKEYCODE_SOFT_LEFT:
                case AKEYCODE_SYSTEM_NAVIGATION_LEFT:
                    return core::event::button::KeyboardKeyId::Left;
                case AKEYCODE_DPAD_RIGHT:
                case AKEYCODE_META_RIGHT:
                case AKEYCODE_SOFT_RIGHT:
                case AKEYCODE_SYSTEM_NAVIGATION_RIGHT:
                    return core::event::button::KeyboardKeyId::Right;
                case AKEYCODE_DPAD_UP:
                case AKEYCODE_SYSTEM_NAVIGATION_UP:
                    return core::event::button::KeyboardKeyId::Up;
                case AKEYCODE_DPAD_DOWN:
                case AKEYCODE_SYSTEM_NAVIGATION_DOWN:
                    return core::event::button::KeyboardKeyId::Down;
                case AKEYCODE_ESCAPE:
                    return core::event::button::KeyboardKeyId::Escape;
                case AKEYCODE_A:
                    return core::event::button::KeyboardKeyId::A;
                case AKEYCODE_B:
                    return core::event::button::KeyboardKeyId::B;
                case AKEYCODE_C:
                    return core::event::button::KeyboardKeyId::C;
                case AKEYCODE_D:
                    return core::event::button::KeyboardKeyId::D;
                case AKEYCODE_E:
                    return core::event::button::KeyboardKeyId::E;
                case AKEYCODE_F:
                    return core::event::button::KeyboardKeyId::F;
                case AKEYCODE_G:
                    return core::event::button::KeyboardKeyId::G;
                case AKEYCODE_H:
                    return core::event::button::KeyboardKeyId::H;
                case AKEYCODE_I:
                    return core::event::button::KeyboardKeyId::I;
                case AKEYCODE_J:
                    return core::event::button::KeyboardKeyId::J;
                case AKEYCODE_K:
                    return core::event::button::KeyboardKeyId::K;
                case AKEYCODE_L:
                    return core::event::button::KeyboardKeyId::L;
                case AKEYCODE_M:
                    return core::event::button::KeyboardKeyId::M;
                case AKEYCODE_N:
                    return core::event::button::KeyboardKeyId::N;
                case AKEYCODE_O:
                    return core::event::button::KeyboardKeyId::O;
                case AKEYCODE_P:
                    return core::event::button::KeyboardKeyId::P;
                case AKEYCODE_Q:
                    return core::event::button::KeyboardKeyId::Q;
                case AKEYCODE_R:
                    return core::event::button::KeyboardKeyId::R;
                case AKEYCODE_S:
                    return core::event::button::KeyboardKeyId::S;
                case AKEYCODE_T:
                    return core::event::button::KeyboardKeyId::T;
                case AKEYCODE_U:
                    return core::event::button::KeyboardKeyId::U;
                case AKEYCODE_V:
                    return core::event::button::KeyboardKeyId::V;
                case AKEYCODE_W:
                    return core::event::button::KeyboardKeyId::W;
                case AKEYCODE_X:
                    return core::event::button::KeyboardKeyId::X;
                case AKEYCODE_Y:
                    return core::event::button::KeyboardKeyId::Y;
                case AKEYCODE_Z:
                    return core::event::button::KeyboardKeyId::Z;
                case AKEYCODE_0:
                    return core::event::button::KeyboardKeyId::Num0;
                case AKEYCODE_1:
                    return core::event::button::KeyboardKeyId::Num1;
                case AKEYCODE_2:
                    return core::event::button::KeyboardKeyId::Num2;
                case AKEYCODE_3:
                    return core::event::button::KeyboardKeyId::Num3;
                case AKEYCODE_4:
                    return core::event::button::KeyboardKeyId::Num4;
                case AKEYCODE_5:
                    return core::event::button::KeyboardKeyId::Num5;
                case AKEYCODE_6:
                    return core::event::button::KeyboardKeyId::Num6;
                case AKEYCODE_7:
                    return core::event::button::KeyboardKeyId::Num7;
                case AKEYCODE_8:
                    return core::event::button::KeyboardKeyId::Num8;
                case AKEYCODE_9:
                    return core::event::button::KeyboardKeyId::Num9;
                case AKEYCODE_NUMPAD_0:
                    return core::event::button::KeyboardKeyId::Numpad0;
                case AKEYCODE_NUMPAD_1:
                    return core::event::button::KeyboardKeyId::Numpad1;
                case AKEYCODE_NUMPAD_2:
                    return core::event::button::KeyboardKeyId::Numpad2;
                case AKEYCODE_NUMPAD_3:
                    return core::event::button::KeyboardKeyId::Numpad3;
                case AKEYCODE_NUMPAD_4:
                    return core::event::button::KeyboardKeyId::Numpad4;
                case AKEYCODE_NUMPAD_5:
                    return core::event::button::KeyboardKeyId::Numpad5;
                case AKEYCODE_NUMPAD_6:
                    return core::event::button::KeyboardKeyId::Numpad6;
                case AKEYCODE_NUMPAD_7:
                    return core::event::button::KeyboardKeyId::Numpad7;
                case AKEYCODE_NUMPAD_8:
                    return core::event::button::KeyboardKeyId::Numpad8;
                case AKEYCODE_NUMPAD_9:
                    return core::event::button::KeyboardKeyId::Numpad9;
                case AKEYCODE_SPACE:
                    return core::event::button::KeyboardKeyId::Space;
                case AKEYCODE_LEFT_BRACKET:
                    return core::event::button::KeyboardKeyId::LeftBracket;
                case AKEYCODE_RIGHT_BRACKET:
                    return core::event::button::KeyboardKeyId::RightBracket;
                case AKEYCODE_GRAVE:
                    return core::event::button::KeyboardKeyId::Tilda;
                case AKEYCODE_DEL:
                    return core::event::button::KeyboardKeyId::Backspace;
                case AKEYCODE_BACKSLASH:
                    return core::event::button::KeyboardKeyId::Backslash;
                case AKEYCODE_SLASH:
                    return core::event::button::KeyboardKeyId::Slash;
                case AKEYCODE_APOSTROPHE:
                    return core::event::button::KeyboardKeyId::Quote;
                case AKEYCODE_SEMICOLON:
                    return core::event::button::KeyboardKeyId::Semicolon;
                case AKEYCODE_MINUS:
                    return core::event::button::KeyboardKeyId::Minus;
                case AKEYCODE_EQUALS:
                    return core::event::button::KeyboardKeyId::Equal;
                case AKEYCODE_TAB:
                    return core::event::button::KeyboardKeyId::Tab;
                case AKEYCODE_CAPS_LOCK:
                    return core::event::button::KeyboardKeyId::CapsLock;
                case AKEYCODE_SHIFT_LEFT:
                    return core::event::button::KeyboardKeyId::LeftShift;
                case AKEYCODE_SHIFT_RIGHT:
                    return core::event::button::KeyboardKeyId::RightShift;
                case AKEYCODE_CTRL_LEFT:
                    return core::event::button::KeyboardKeyId::LeftControl;
                case AKEYCODE_CTRL_RIGHT:
                    return core::event::button::KeyboardKeyId::RightControl;
                case AKEYCODE_ALT_LEFT:
                    return core::event::button::KeyboardKeyId::LeftAlt;
                case AKEYCODE_ALT_RIGHT:
                    return core::event::button::KeyboardKeyId::RightAlt;
                case AKEYCODE_MENU:
                    return core::event::button::KeyboardKeyId::Menu;
                case AKEYCODE_COMMA:
                    return core::event::button::KeyboardKeyId::Comma;
                case AKEYCODE_PERIOD:
                    return core::event::button::KeyboardKeyId::Dot;
                case AKEYCODE_F1:
                    return core::event::button::KeyboardKeyId::F1;
                case AKEYCODE_F2:
                    return core::event::button::KeyboardKeyId::F2;
                case AKEYCODE_F3:
                    return core::event::button::KeyboardKeyId::F3;
                case AKEYCODE_F4:
                    return core::event::button::KeyboardKeyId::F4;
                case AKEYCODE_F5:
                    return core::event::button::KeyboardKeyId::F5;
                case AKEYCODE_F6:
                    return core::event::button::KeyboardKeyId::F6;
                case AKEYCODE_F7:
                    return core::event::button::KeyboardKeyId::F7;
                case AKEYCODE_F8:
                    return core::event::button::KeyboardKeyId::F8;
                case AKEYCODE_F9:
                    return core::event::button::KeyboardKeyId::F9;
                case AKEYCODE_F10:
                    return core::event::button::KeyboardKeyId::F10;
                case AKEYCODE_F11:
                    return core::event::button::KeyboardKeyId::F11;
                case AKEYCODE_F12:
                    return core::event::button::KeyboardKeyId::F12;
                case AKEYCODE_INSERT:
                    return core::event::button::KeyboardKeyId::Insert;
                case AKEYCODE_FORWARD_DEL:
                    return core::event::button::KeyboardKeyId::Delete;
                case AKEYCODE_HOME:
                    return core::event::button::KeyboardKeyId::Home;
                case AKEYCODE_MOVE_END:
                    return core::event::button::KeyboardKeyId::End;
                case AKEYCODE_PAGE_DOWN:
                    return core::event::button::KeyboardKeyId::PageDown;
                case AKEYCODE_PAGE_UP:
                    return core::event::button::KeyboardKeyId::PageUp;
                case AKEYCODE_SCROLL_LOCK:
                    return core::event::button::KeyboardKeyId::ScrollLock;
                case AKEYCODE_MEDIA_PAUSE:
                    return core::event::button::KeyboardKeyId::Pause;
                case AKEYCODE_NUM_LOCK:
                    return core::event::button::KeyboardKeyId::NumpadLock;
                case AKEYCODE_NUMPAD_DIVIDE:
                    return core::event::button::KeyboardKeyId::NumpadSlash;
                case AKEYCODE_NUMPAD_MULTIPLY:
                    return core::event::button::KeyboardKeyId::NumpadStar;
                case AKEYCODE_NUMPAD_SUBTRACT:
                    return core::event::button::KeyboardKeyId::NumpadMinus;
                case AKEYCODE_NUMPAD_ADD:
                    return core::event::button::KeyboardKeyId::NumpadPlus;
                case AKEYCODE_NUMPAD_ENTER:
                    return core::event::button::KeyboardKeyId::NumpadEnter;
                case AKEYCODE_NUMPAD_DOT:
                    return core::event::button::KeyboardKeyId::NumpadDot;
                default:
                    GXLOGD("Unhandled mouse button, left button returned instead. " << static_cast<int>(key))
                    return core::event::button::KeyboardKeyId::Unknown;
            } }())));
        break;
    }
    case AINPUT_EVENT_TYPE_MOTION: {
        const auto action = AMotionEvent_getAction(e);
        const auto flags = action & AMOTION_EVENT_ACTION_MASK;
        const auto pointer_index = static_cast<size_t>((action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT);
        const auto pointer_id = AMotionEvent_getPointerId(e, pointer_index);
        const auto pointer_counts = AMotionEvent_getPointerCount(e);
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
            for (auto pi = decltype(pointer_counts) { 0 }; pi < pointer_counts; ++pi) {
                event_engine->touch_move(
                    static_cast<core::event::touch::FingerId>(AMotionEvent_getPointerId(e, pi)),
                    static_cast<int>(AMotionEvent_getRawX(e, pi)),
                    event_engine->get_window_height() - static_cast<int>(AMotionEvent_getRawY(e, pi)));
            }
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

void gearoenix::platform::Application::handle_cmd(android_app* const a, int32_t cmd) noexcept
{
    auto sys_app = static_cast<Application*>(a->userData);
    sys_app->handle(a, cmd);
}

int32_t gearoenix::platform::Application::handle_input(android_app* const a, AInputEvent* const e) noexcept
{
    auto sys_app = static_cast<Application*>(a->userData);
    return sys_app->handle(a, e);
}

void gearoenix::platform::Application::on_check_ready_to_render(android_app* const a) noexcept
{
    if (running)
        return;
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
        // core::event::platform::System eul(core::event::platform::System::Action::UNLOAD);
        // core_app->on_event(eul);
        // render_engine->on_event(eul);
        // gl_ctx->Invalidate();
        // and_app = a;
        // gl_ctx->Init(a->window);
        // core::event::platform::System erl(core::event::platform::System::Action::RELOAD);
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

void gearoenix::platform::Application::on_not_ready_to_render() noexcept
{
    if (!running)
        return;
    if (resumed && surface_ready)
        return;
    running = false;
#ifdef GX_USE_OPENGL
    if (GX_RUNTIME_USE_OPENGL_E(render_engine)) {
        gl_context->suspend();
    }
#endif
}

gearoenix::platform::Application::Application(android_app* and_app) noexcept
    : android_application(and_app)
    , event_engine(new core::event::Engine())
    , gl_context(new GlContext())
{
    and_app->userData = this;
    and_app->onAppCmd = gearoenix::platform::Application::handle_cmd;
    and_app->onInputEvent = gearoenix::platform::Application::handle_input;
    int events;
    android_poll_source* source;
    do {
        if (ALooper_pollAll(-1, nullptr, &events, (void**)&source) >= 0) {
            if (source != nullptr)
                source->process(and_app, source);
        }
        if (render_engine != nullptr) {
            event_engine->initialize_render_engine(render_engine.get());
            break;
        }
    } while (and_app->destroyRequested == 0);
}

gearoenix::platform::Application::~Application() noexcept
{
    core_application = nullptr;
    event_engine = nullptr;
    asset_manager = nullptr;
    render_engine = nullptr;
    gl_context = nullptr;
    android_application = nullptr;
}

void gearoenix::platform::Application::execute(std::unique_ptr<core::Application> app) noexcept
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

const char* gearoenix::platform::Application::get_clipboard() const noexcept
{
    return nullptr;
}

void gearoenix::platform::Application::set_soft_keyboard_visibility(const bool show) noexcept
{
    // Attaches the current thread to the JVM.
    auto* const activity = android_application->activity;
    JavaVM* const java_vm = activity->vm;
    JNIEnv* jni_env = activity->env;
    JavaVMAttachArgs java_vm_attach_args;
    java_vm_attach_args.version = JNI_VERSION_1_6;
    java_vm_attach_args.name = "NativeThread";
    java_vm_attach_args.group = nullptr;
    if (JNI_ERR == java_vm->AttachCurrentThread(&jni_env, &java_vm_attach_args)) {
        GXLOGE("Error in attaching current thread to JVM")
        return;
    }
    jobject native_activity = activity->clazz;
    jclass class_native_activity = jni_env->GetObjectClass(native_activity);
    jmethodID method = jni_env->GetMethodID(class_native_activity, show ? "showKeyboard" : "hideKeyboard", "()V");
    jni_env->CallVoidMethod(native_activity, method);
    java_vm->DetachCurrentThread();
}

#endif