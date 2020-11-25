#ifndef GEAROENIX_PLATFORM_APPLICATION_HPP
#define GEAROENIX_PLATFORM_APPLICATION_HPP
#include "../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-plt-build-configuration.hpp"
#include "gx-plt-runtime-configuration.hpp"

namespace gearoenix::platform {
struct BaseApplication final {
    GX_GET_CREF_PRV(RuntimeConfiguration, configuration)
};
}

#if defined(GX_PLT_ANDROID)
#include "android/gx-sys-and-app.hpp"
#elif defined(GX_PLT_WINAPI)
#include "winapi/gx-plt-win-application.hpp"
#else
#error "Unspecified platform application interface."
#endif
#endif // GEAROENIX_SYSTEM_APPLICATION_HPP
