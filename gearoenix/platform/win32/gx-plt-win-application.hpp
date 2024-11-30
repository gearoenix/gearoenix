#pragma once
#include "../gx-plt-build-configuration.hpp"
#ifdef GX_PLATFORM_INTERFACE_WIN32
#include "../gx-plt-application.hpp"
#include <Windows.h>

namespace gearoenix::core {
struct Application;
}

namespace gearoenix::platform {
struct Application final {
    GX_GET_VAL_PRV(HINSTANCE, instance, nullptr)
    GX_GET_VAL_PRV(HWND, window, nullptr)
    GX_GET_REF_PRV(BaseApplication, base)
private:
    [[nodiscard]] static LRESULT CALLBACK static_handler(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param);
    [[nodiscard]] LRESULT handler(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param);

    void update_window_size();
    void update_mouse_position();

public:
    Application(GX_MAIN_ENTRY_ARGS_DEF, const RuntimeConfiguration& config = RuntimeConfiguration());
    ~Application();
    void run(core::Application* = nullptr);
};
}
#endif