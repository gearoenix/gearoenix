#include "gx-plt-and-application.hpp"
#ifdef GX_PLATFORM_INTERFACE_ANDROID
#include "../../core/gx-cr-application.hpp"
#include "../../render/engine/gx-rnd-eng-engine.hpp"
#include "../../render/gx-rnd-build-configuration.hpp"
#include "../gx-plt-log.hpp"
#include "gx-plt-and-key.hpp"

#ifdef GX_RENDER_OPENGL_ENABLED
#include "gx-plt-gl-context.hpp"
#endif

void gearoenix::platform::Application::handle(android_app* const a, int32_t cmd) noexcept
{
    switch (cmd) {
    case APP_CMD_START:
        GX_LOG_D("Application started.");
        on_check_ready_to_render(a);
        break;
    case APP_CMD_RESUME:
        GX_LOG_D("Application resumed.");
        resumed = true;
        on_check_ready_to_render(a);
        break;
    case APP_CMD_PAUSE:
        GX_LOG_D("Application paused.");
        resumed = false;
        on_not_ready_to_render();
        break;
    case APP_CMD_STOP:
        GX_LOG_D("Application stopped.");
        resumed = false;
        on_not_ready_to_render();
        break;
    case APP_CMD_DESTROY:
        GX_LOG_D("Application destroyed.");
        break;
    case APP_CMD_GAINED_FOCUS:
        GX_LOG_D("Application focused.");
        focused = true;
        on_check_ready_to_render(a);
        break;
    case APP_CMD_LOST_FOCUS:
        GX_LOG_D("Application unfocused.");
        focused = false;
        on_not_ready_to_render();
        break;
    case APP_CMD_INIT_WINDOW:
        GX_LOG_D("Application surface ready.");
        surface_ready = true;
        on_check_ready_to_render(a);
        break;
    case APP_CMD_TERM_WINDOW:
        GX_LOG_D("Android window terminated.");
        surface_ready = false;
        on_not_ready_to_render();
        break;
    case APP_CMD_WINDOW_RESIZED:
        GX_LOG_D("Android window resized.");
        // todo
        break;
    case APP_CMD_CONFIG_CHANGED:
        GX_LOG_D("Android config changed.");
        // todo
        break;
    default:
        GX_LOG_D("event not handled: " << cmd);
    }
}

int32_t gearoenix::platform::Application::handle(android_app* const, AInputEvent* const e) noexcept
{
    const auto event_type = AInputEvent_getType(e);
    switch (event_type) {
    case AINPUT_EVENT_TYPE_KEY:
        base.keyboard_key(convert_android_key(AKeyEvent_getKeyCode(e)), [&]() noexcept {
            switch (AKeyEvent_getAction(e)) {
                case AKEY_EVENT_ACTION_UP:
                    return gearoenix::platform::key::Action::Release;
                case AKEY_EVENT_ACTION_DOWN:
                    return gearoenix::platform::key::Action::Press;
                default:
                    GX_LOG_E("Unhandled key action.");
                    return gearoenix::platform::key::Action::Press;
            } }());
        break;
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
            base.touch_down(
                static_cast<FingerId>(pointer_id),
                static_cast<double>(x),
                base.window_size.y - static_cast<double>(y));
            break;
        case AMOTION_EVENT_ACTION_MOVE:
            for (auto pi = decltype(pointer_counts) { 0 }; pi < pointer_counts; ++pi) {
                base.touch_move(
                    static_cast<FingerId>(AMotionEvent_getPointerId(e, pi)),
                    static_cast<double>(AMotionEvent_getRawX(e, pi)),
                    base.window_size.y - static_cast<double>(AMotionEvent_getRawY(e, pi)));
            }
            break;
        case AMOTION_EVENT_ACTION_UP:
        case AMOTION_EVENT_ACTION_POINTER_UP:
            base.touch_up(static_cast<FingerId>(pointer_id));
            break;
        case AMOTION_EVENT_ACTION_CANCEL:
            base.touch_cancel(static_cast<FingerId>(pointer_id));
            break;
        default:
            GX_LOG_E("Unhandled motion event flag: " << flags);
            break;
        }
        break;
    }
    default:
        GX_LOG_E("Unexpected event type value: " << event_type);
        break;
    }
    return 0;
}

void gearoenix::platform::Application::handle_cmd(android_app* const a, int32_t cmd) noexcept
{
    auto platform_application = static_cast<Application*>(a->userData);
    platform_application->handle(a, cmd);
}

int32_t gearoenix::platform::Application::handle_input(android_app* const a, AInputEvent* const e) noexcept
{
    auto platform_application = static_cast<Application*>(a->userData);
    return platform_application->handle(a, e);
}

void gearoenix::platform::Application::on_check_ready_to_render(android_app* const a) noexcept
{
    if (running)
        return;
    if (!resumed || !focused || !surface_ready)
        return;
    if (android_application->window == nullptr)
        return;
    if (nullptr == base.render_engine) {
#ifdef GX_RENDER_OPENGL_ENABLED
        if (gl_context != nullptr) {
            gl_context->init(android_application->window);
            base.initialize_window_size(
                ANativeWindow_getWidth(android_application->window),
                ANativeWindow_getHeight(android_application->window));
            base.initialize_engine(*this);
        }
#endif
    } else if (a->window != android_application->window) {
        GX_LOG_F("Window reinitialized with different window (not implemented).");
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
        GX_LOG_D("Window Reinitialized");
#ifdef GX_RENDER_OPENGL_ENABLED
        if (base.render_engine->get_engine_type() == render::engine::Type::OpenGL) {
            gl_context->resume(android_application->window);
            // TODO
            //            event_engine->initialize_window_size(
            //                static_cast<std::size_t>(gl_context->get_screen_width()),
            //                static_cast<std::size_t>(gl_context->get_screen_height()));
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
#ifdef GX_RENDER_OPENGL_ENABLED
    if (base.render_engine->get_engine_type() == render::engine::Type::OpenGL) {
        gl_context->suspend();
    }
#endif
}

gearoenix::platform::Application::Application(GX_MAIN_ENTRY_ARGS_DEF, const RuntimeConfiguration& config) noexcept
    : base(GX_MAIN_ENTRY_ARGS, config)
    , android_application(state)
    , gl_context(new GlContext())
{
    state->userData = this;
    state->onAppCmd = gearoenix::platform::Application::handle_cmd;
    state->onInputEvent = gearoenix::platform::Application::handle_input;
    int events;
    android_poll_source* source;
    do {
        if (ALooper_pollAll(-1, nullptr, &events, (void**)&source) >= 0) {
            if (source != nullptr)
                source->process(state, source);
        }
        if (nullptr != base.render_engine) {
            break;
        }
    } while (state->destroyRequested == 0);
}

gearoenix::platform::Application::~Application() noexcept = default;

void gearoenix::platform::Application::run(core::Application* const core_app) noexcept
{
    base.initialize_core_application(*this, core_app);
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
            base.update();
        }
    } while (android_application->destroyRequested == 0);
    running = false;
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
        GX_LOG_E("Error in attaching current thread to JVM");
        return;
    }
    jobject native_activity = activity->clazz;
    jclass class_native_activity = jni_env->GetObjectClass(native_activity);
    jmethodID method = jni_env->GetMethodID(class_native_activity, show ? "showKeyboard" : "hideKeyboard", "()V");
    jni_env->CallVoidMethod(native_activity, method);
    java_vm->DetachCurrentThread();
}

#endif