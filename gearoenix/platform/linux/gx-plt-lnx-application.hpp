#ifndef GEAROENIX_PLATFORM_LINUX_APPLICATION_HPP
#define GEAROENIX_PLATFORM_LINUX_APPLICATION_HPP
#include "../gx-plt-build-configuration.hpp"

#ifdef GX_PLATFORM_LINUX

#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-plt-application.hpp"
#include "../gx-plt-main-entry.hpp"
#include "../gx-plt-runtime-configuration.hpp"
#include <X11/Xlib.h>
#include <memory>

namespace gearoenix::core {
struct Application;
}

namespace gearoenix::platform {
struct Application final {
    GX_GET_CREF_PRV(BaseApplication, base)
    GX_GET_PTR_PRV(Display, display)
    GX_GET_VAL_PRV(Window, window, 0)

private:
    Atom close_message;

    [[nodiscard]] bool fetch_events(XEvent& event) noexcept;

public:
    explicit Application(GX_MAIN_ENTRY_ARGS_DEF, const RuntimeConfiguration& config = RuntimeConfiguration()) noexcept;
    ~Application() noexcept;

    void run() noexcept;
};
}
#endif
#endif