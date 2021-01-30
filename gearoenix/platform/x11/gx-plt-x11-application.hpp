#ifndef GEAROENIX_PLATFORM_X11_APPLICATION_HPP
#define GEAROENIX_PLATFORM_X11_APPLICATION_HPP
#include "../gx-plt-build-configuration.hpp"
#ifdef GX_PLATFORM_INTERAFCE_X11

#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-plt-application.hpp"
#include "../gx-plt-main-entry.hpp"
#include "../gx-plt-runtime-configuration.hpp"
#include <X11/Xlib.h>

namespace gearoenix::core {
struct Application;
}

namespace gearoenix::platform {
struct Application final {
    GX_GET_REF_PRV(BaseApplication, base)
    GX_GET_PTRC_PRV(Display, display)
    GX_GET_PTRC_PRV(Screen, screen)
    GX_GET_CVAL_PRV(Window, root_window)
    GX_GET_CVAL_PRV(unsigned long, black_pixel)
    GX_GET_CVAL_PRV(Window, window)
private:
    Atom close_message;

    void fetch_events() noexcept;

public:
    explicit Application(GX_MAIN_ENTRY_ARGS_DEF, const RuntimeConfiguration& config = RuntimeConfiguration()) noexcept;
    ~Application() noexcept;

    void run(core::Application* core_application = nullptr) noexcept;
};
}
#endif
#endif