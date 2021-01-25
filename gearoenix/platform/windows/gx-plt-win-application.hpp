#ifndef GEAROENIX_PLATFORM_WINDOWS_APPLICATION_HPP
#define GEAROENIX_PLATFORM_WINDOWS_APPLICATION_HPP
#include "../gx-plt-build-configuration.hpp"
#ifdef GX_PLATFORM_WINDOWS
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-plt-application.hpp"
#include "../gx-plt-runtime-configuration.hpp"
#include <Windows.h>
#include <memory>

namespace gearoenix::core {
struct Application;
}

namespace gearoenix::platform {
struct Application final {
    GX_GET_VAL_PRV(HINSTANCE, instance, nullptr)
    GX_GET_VAL_PRV(HWND, window, nullptr)
    GX_GET_REF_PRV(BaseApplication, base)
private:
    [[nodiscard]] static LRESULT CALLBACK static_handler(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) noexcept;
    [[nodiscard]] LRESULT handler(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) noexcept;

public:
    Application(GX_MAIN_ENTRY_ARGS_DEF, const RuntimeConfiguration& config = RuntimeConfiguration()) noexcept;
    ~Application() noexcept;
    void run(core::Application* = nullptr) noexcept;
};
}
#endif
#endif