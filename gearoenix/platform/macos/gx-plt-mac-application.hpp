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
    GX_GET_PTRC_PRV(GearoenixPlatformViewController, view_controller)
    
private:
    void update_window_size() noexcept;
    void update_mouse_position() noexcept;

public:
    Application(GX_MAIN_ENTRY_ARGS_DEF, const RuntimeConfiguration& config = RuntimeConfiguration()) noexcept;
    ~Application() noexcept;
    void run(core::Application* = nullptr) noexcept;
    void update() noexcept;
};
}
#endif
#endif
