#ifndef GEAROENIX_PLATFORM_APPLICATION_HPP
#define GEAROENIX_PLATFORM_APPLICATION_HPP
#include "gx-plt-build-configuration.hpp"

#if defined(GX_PLT_ANDROID)
#include "android/gx-sys-and-app.hpp"
#elif defined(GX_PLT_WINAPI)
#include "winapi/gx-plt-win-application.hpp"
#else
#error "Unspecified platform application interface."
#endif
#endif // GEAROENIX_SYSTEM_APPLICATION_HPP
