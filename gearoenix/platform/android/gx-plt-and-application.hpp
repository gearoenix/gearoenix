#ifndef GEAROENIX_SYSTEM_ANDROID_APPLICATION_HPP
#define GEAROENIX_SYSTEM_ANDROID_APPLICATION_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#if defined(GX_IN_ANDROID) && !defined(GX_USE_SDL)
#include "../../core/gx-cr-static.hpp"
#include "../../core/gx-cr-types.hpp"
#include "../../render/engine/gx-rnd-eng-type.hpp"
#include "../gx-plt-configuration.hpp"

struct android_app;
struct AInputEvent;

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::core::asset {
struct Manager;
}

namespace gearoenix::core::event {
struct Engine;
}

namespace gearoenix::core {
struct Application;
}

namespace gearoenix::platform {
struct File;
struct GlContext;
struct Application {
    GX_GET_REF_PRV(Configuration, configuration)
    GX_GET_UPTR_PRV(android_app, android_application)
    GX_GET_UPTR_PRV(render::engine::Engine, render_engine)
    GX_GET_UPTR_PRV(core::Application, core_application)
    GX_GET_UPTR_PRV(core::asset::Manager, asset_manager)
    GX_GET_UPTR_PRV(core::event::Engine, event_engine)
    GX_GET_VAL_PRV(bool, running, false)
    GX_GET_VAL_PRV(bool, resumed, false)
    GX_GET_VAL_PRV(bool, focused, false)
    GX_GET_VAL_PRV(bool, surface_ready, false)
#ifdef GX_USE_OPENGL
    GX_GET_UPTR_PRV(GlContext, gl_context)
#endif

private:
    void handle(android_app* app, int32_t cmd) noexcept;
    int32_t handle(android_app* app, AInputEvent* e) noexcept;
    static void handle_cmd(android_app* app, int32_t cmd) noexcept;
    static int32_t handle_input(android_app* app, AInputEvent* e) noexcept;
    void on_check_ready_to_render(android_app*) noexcept;
    void on_not_ready_to_render() noexcept;

public:
    Application(struct android_app* and_app) noexcept;
    ~Application() noexcept;
    void execute(std::unique_ptr<core::Application> core_app) noexcept;
    [[nodiscard]] const char* get_clipboard() const noexcept;
    void set_soft_keyboard_visibility(bool show) noexcept;
};
}

#endif
#endif
