#ifndef GEAROENIX_PLATFORM_MACOS_APPLICATION_HPP
#define GEAROENIX_PLATFORM_MACOS_APPLICATION_HPP
#include "../gx-plt-build-configuration.hpp"
#ifdef GX_PLATFORM_MACOS
#include "../gx-plt-application.hpp"
#import "gx-plt-mac-app-delegate.hpp"
#import "gx-plt-mac-view-controller.hpp"

namespace gearoenix::core {
struct Application;
}

namespace gearoenix::platform {

struct Application final {
    GX_GET_REF_PRV(BaseApplication, base)
    GX_GET_PTRC_PRV(GearoenixPlatformAppDelegate, app_delegate)

private:
    void update_window_size();
    void update_mouse_position();

public:
    Application(GX_MAIN_ENTRY_ARGS_DEF, const RuntimeConfiguration& config = RuntimeConfiguration());
    ~Application();
    void run(core::Application* = nullptr);
    void update();
};
}
#endif
#endif
