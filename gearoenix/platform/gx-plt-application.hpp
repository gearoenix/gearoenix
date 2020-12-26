#ifndef GEAROENIX_PLATFORM_APPLICATION_HPP
#define GEAROENIX_PLATFORM_APPLICATION_HPP
#include "../core/macro/gx-cr-mcr-getter-setter.hpp"
//#include "../core/event/gx-cr-ev-event.hpp"
#include "gx-plt-build-configuration.hpp"
#include "gx-plt-runtime-configuration.hpp"
#include <memory>

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::platform {
struct Application;
struct BaseApplication final {
    friend struct Application;

    GX_GET_CREF_PRV(RuntimeConfiguration, configuration)

    GX_GET_VAL_PRV(bool, running, true)

    GX_GET_VAL_PRV(int, window_width, -1)
    GX_GET_VAL_PRV(int, window_height, -1)
    GX_GET_VAL_PRV(double, window_aspect_ratio, 1)
    GX_GET_VAL_PRV(int, pre_window_width, -1)
    GX_GET_VAL_PRV(int, pre_window_height, -1)
    GX_GET_VAL_PRV(double, pre_window_aspect_ratio, 1)
    GX_GET_VAL_PRV(int, delta_window_width, 0)
    GX_GET_VAL_PRV(int, delta_window_height, 0)
    GX_GET_VAL_PRV(double, delta_window_aspect_ratio, 1)
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

    GX_GET_CREF_PRV(std::shared_ptr<render::engine::Engine>, render_engine)

private:
    BaseApplication(const RuntimeConfiguration& configuration) noexcept;

    void initialize_window_position(int x, int y) noexcept;

    void initialize_window_size(int w, int h) noexcept;
    void update_window_size(int w, int h) noexcept;

    void initialize_mouse_position(double x, double y) noexcept;
    void update_mouse_position(double x, double y) noexcept;

    void going_to_be_closed() noexcept;
};
}

#if defined(GX_PLT_ANDROID)
#include "android/gx-sys-and-app.hpp"
#elif defined(GX_PLATFORM_WINDOWS)
#include "windows/gx-plt-win-application.hpp"
#elif defined(GX_PLATFORM_LINUX)
#include "linux/gx-plt-lnx-application.hpp"
#else
#error "Unspecified platform application interface."
#endif
#endif // GEAROENIX_SYSTEM_APPLICATION_HPP
