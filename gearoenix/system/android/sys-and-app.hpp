#ifndef GEAROENIX_SYSTEM_ANDROID_APPLICATION_HPP
#define GEAROENIX_SYSTEM_ANDROID_APPLICATION_HPP
#include "../../core/cr-build-configuration.hpp"
#if defined(GX_IN_ANDROID) && !defined(GX_USE_SDL)
#include "../../core/cr-static.hpp"
#include "../../core/cr-types.hpp"
#include "../../render/engine/rnd-eng-type.hpp"
#include "../sys-configuration.hpp"

struct android_app;
struct AInputEvent;

namespace gearoenix::render::engine {
class Engine;
}

namespace gearoenix::core::asset {
class Manager;
}

namespace gearoenix::core::event {
class Engine;
}

namespace gearoenix::core {
class Application;
}

namespace gearoenix::system {
class File;
class GlContext;
class Application {
    GX_GET_REF_PRV(Configuration, configuration)
    GX_GET_UPTR_PRV(android_app, android_application)
    GX_GET_UPTR_PRV(render::engine::Engine, render_engine)
    GX_GET_UPTR_PRV(core::Application, core_application)
    GX_GET_UPTR_PRV(core::asset::Manager, asset_manager)
    GX_GET_UPTR_PRV(core::event::Engine, event_engine)
    GX_GET_VAL_PRV(unsigned int, window_width, 0)
    GX_GET_VAL_PRV(unsigned int, window_height, 0)
    GX_GET_VAL_PRV(double, window_ratio, 1.0)
private:
    bool running = true;
#ifdef GX_USE_OPENGL
    GlContext* gl_context = nullptr;
#endif
    double half_height_inverted = 1.0;
    double x = 0.0;
    double y = 0.0;
    void handle(android_app* app, int32_t cmd) noexcept;
    int32_t handle(android_app* app, AInputEvent* e) noexcept;
    static void handle_cmd(android_app* app, int32_t cmd) noexcept;
    static int32_t handle_input(android_app* app, AInputEvent* e) noexcept;
    void compute_screen_ratios() noexcept;

public:
    Application(struct android_app* and_app) noexcept;
    ~Application() noexcept;
    void execute(std::unique_ptr<core::Application> core_app) noexcept;
    [[nodiscard]] double convert_x_to_ratio(int x) const noexcept;
    [[nodiscard]] double convert_y_to_ratio(int y) const noexcept;
    [[nodiscard]] const char* get_clipboard() const noexcept;
};
}

#endif
#endif
