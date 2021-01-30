#ifndef GEAROENIX_PLATFORM_SDL2_APPLICATION_HPP
#define GEAROENIX_PLATFORM_SDL2_APPLICATION_HPP
#include "../gx-plt-build-configuration.hpp"
#ifdef GX_PLATFORM_INTERFACE_SDL2
#include "../gx-plt-application.hpp"
#include <SDL.h>

namespace gearoenix::platform {
struct Application final {
    //    GX_GET_VAL_PRV(HINSTANCE, instance, nullptr)
    //    GX_GET_VAL_PRV(HWND, window, nullptr)
    GX_GET_REF_PRV(BaseApplication, base)
private:
    void update_window_size() noexcept;
    void update_mouse_position() noexcept;

public:
    Application(GX_MAIN_ENTRY_ARGS_DEF, const RuntimeConfiguration& config = RuntimeConfiguration()) noexcept;
    ~Application() noexcept;
    void run(core::Application* = nullptr) noexcept;
};
}
#endif
#endif