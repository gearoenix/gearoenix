#ifndef GEAROENIX_PLATFORM_WINAPI_APPLICATION_HPP
#define GEAROENIX_PLATFORM_WINAPI_APPLICATION_HPP
#include "../gx-plt-build-configuration.hpp"

#ifdef GX_PLT_WINAPI

#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include <windef.h>
namespace gearoenix::platform {
struct Application final {
    GX_GET_VAL_PRV(HINSTANCE, instance, nullptr)
    GX_GET_VAL_PRV(HWND, window, nullptr)
private:
    [[nodiscard]] static LRESULT CALLBACK static_handler(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) noexcept;
    [[nodiscard]] LRESULT handler(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) noexcept;

public:
    Application() noexcept;
    ~Application() noexcept;
    void run() noexcept;
};
}
#endif
#endif