#ifndef GEAROENIX_PLATFORM_ANDROID_APPLICATION_HPP
#define GEAROENIX_PLATFORM_ANDROID_APPLICATION_HPP
#include "../gx-plt-build-configuration.hpp"
#ifdef GX_PLATFORM_INTERFACE_ANDROID
#include "../../render/gx-rnd-build-configuration.hpp"
#include "../gx-plt-application.hpp"

namespace gearoenix::core {
struct Application;
}

namespace gearoenix::platform {
struct File;
struct GlContext;
struct GooglePlayBilling;

struct Application {
    GX_GET_REF_PRV(BaseApplication, base);
    GX_GET_UPTR_PRV(android_app, android_application);
    GX_GET_VAL_PRV(bool, running, false);
    GX_GET_VAL_PRV(bool, resumed, false);
    GX_GET_VAL_PRV(bool, focused, false);
    GX_GET_VAL_PRV(bool, surface_ready, false);
    GX_GET_UPTR_PRV(GooglePlayBilling, google_play_billing);
#ifdef GX_RENDER_OPENGL_ENABLED
    GX_GET_UPTR_PRV(GlContext, gl_context);
#endif

private:
    void handle(android_app* app, int32_t cmd) noexcept;
    int32_t handle(android_app* app, AInputEvent* e) noexcept;
    static void handle_cmd(android_app* app, int32_t cmd) noexcept;
    static int32_t handle_input(android_app* app, AInputEvent* e) noexcept;
    void on_check_ready_to_render(android_app*) noexcept;
    void on_not_ready_to_render() noexcept;
    void make_thread_current_jni() noexcept;

public:
    Application(GX_MAIN_ENTRY_ARGS_DEF, const RuntimeConfiguration& config = RuntimeConfiguration()) noexcept;
    ~Application() noexcept;
    void run(core::Application* = nullptr) noexcept;
    [[nodiscard]] const char* get_clipboard() const noexcept;
    void set_soft_keyboard_visibility(bool show) noexcept;
};
}

#endif
#endif
