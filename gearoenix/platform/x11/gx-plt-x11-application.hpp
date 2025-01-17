#pragma once
#include "../gx-plt-build-configuration.hpp"
#ifdef GX_PLATFORM_INTERFACE_X11
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
    GX_GET_REF_PRV(BaseApplication, base);
    GX_GET_PTRC_PRV(Display, display);
    GX_GET_PTRC_PRV(Screen, screen);
    GX_GET_CVAL_PRV(Window, root_window);
    GX_GET_CVAL_PRV(unsigned long, black_pixel);
    GX_GET_CVAL_PRV(Window, window);

private:
    Atom close_message;

    void fetch_events();

public:
    explicit Application(GX_MAIN_ENTRY_ARGS_DEF, const RuntimeConfiguration& config = RuntimeConfiguration());
    ~Application();
    void run(core::Application* core_application = nullptr);
#ifdef GX_RENDER_VULKAN_ENABLED
    [[nodiscard]] std::vector<const char*> get_vulkan_extensions() const;
#endif
};
}
#endif