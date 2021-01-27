#ifndef GEAROENIX_PLATFORM_APPLICATION_HPP
#define GEAROENIX_PLATFORM_APPLICATION_HPP
#include "../core/macro/gx-cr-mcr-getter-setter.hpp"
//#include "../core/event/gx-cr-ev-event.hpp"
#include "gx-plt-arguments.hpp"
#include "gx-plt-build-configuration.hpp"
#include "gx-plt-main-entry.hpp"
#include "gx-plt-runtime-configuration.hpp"
#include <chrono>
#include <memory>

namespace gearoenix::core {
struct Application;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::platform {
struct Application;
struct BaseApplication final {
    friend struct Application;

    GX_GET_CREF_PRV(RuntimeConfiguration, configuration)
    GX_GET_CREF_PRV(Arguments, arguments)

    GX_GET_VAL_PRV(bool, running, true)

    GX_GET_VAL_PRV(bool, window_resizing, false)
    GX_GET_VAL_PRV(int, window_width, -1)
    GX_GET_VAL_PRV(int, window_height, -1)
    GX_GET_VAL_PRV(double, window_aspect_ratio, 1)

    GX_GET_VAL_PRV(int, window_x, -1)
    GX_GET_VAL_PRV(int, window_y, -1)
    GX_GET_VAL_PRV(int, pre_window_x, -1)
    GX_GET_VAL_PRV(int, pre_window_y, -1)
    GX_GET_VAL_PRV(int, delta_window_x, 0)
    GX_GET_VAL_PRV(int, delta_window_y, 0)
    GX_GET_VAL_PRV(bool, window_is_up, false)

    GX_GET_VAL_PRV(double, mouse_x, -1.0)
    GX_GET_VAL_PRV(double, mouse_y, -1.0)
    /// From -window_aspect -> +window_aspect
    GX_GET_VAL_PRV(double, mouse_x_nrm, -1.0)
    /// From -1.0 -> +1.0
    GX_GET_VAL_PRV(double, mouse_y_nrm, -1.0)
    GX_GET_VAL_PRV(double, pre_mouse_x, -1.0)
    GX_GET_VAL_PRV(double, pre_mouse_y, -1.0)
    GX_GET_VAL_PRV(double, pre_mouse_x_nrm, -1.0)
    GX_GET_VAL_PRV(double, pre_mouse_y_nrm, -1.0)
    GX_GET_VAL_PRV(double, delta_mouse_x, 0.0)
    GX_GET_VAL_PRV(double, delta_mouse_y, 0.0)
    GX_GET_VAL_PRV(double, delta_mouse_x_nrm, 0.0)
    GX_GET_VAL_PRV(double, delta_mouse_y_nrm, 0.0)

    GX_GET_UPTR_PRV(render::engine::Engine, render_engine)
    GX_GET_UPTR_PRV(core::Application, core_application)

private:
    std::chrono::high_resolution_clock::time_point last_time_window_resized;

private:
    BaseApplication(GX_MAIN_ENTRY_ARGS_DEF, const RuntimeConfiguration& configuration) noexcept;
    ~BaseApplication() noexcept;

    void initialize_window_position(int x, int y) noexcept;

    void initialize_window_size(int w, int h) noexcept;
    void update_window_size(int w, int h) noexcept;

    void initialize_mouse_position(double x, double y) noexcept;
    void update_mouse_position(double x, double y) noexcept;

    void going_to_be_closed() noexcept;
    void update() noexcept;

public:
};
}

#if defined(GX_PLT_ANDROID)
#include "android/gx-plt-and-app.hpp"
#elif defined(GX_PLATFORM_WINDOWS)
#include "windows/gx-plt-win-application.hpp"
#elif defined(GX_PLATFORM_LINUX)
#include "linux/gx-plt-lnx-application.hpp"
#else
#error "Unspecified platform application interface."
#endif
#endif // GEAROENIX_SYSTEM_APPLICATION_HPP
