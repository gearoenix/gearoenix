#ifndef GEAROENIX_SYSTEM_APPLICATION_HPP
#define GEAROENIX_SYSTEM_APPLICATION_HPP
#include "../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_SDL
#include "sdl/gx-sys-sdl-app.hpp"
#elif defined(GX_USE_GLFW)
#include "glfw/gx-sys-glfw.hpp"
#elif defined(GX_IN_ANDROID)
#include "android/gx-sys-and-app.hpp"
#elif defined(GX_USE_WINAPI)
#include "win/gx-sys-win-app.hpp"
#else
#error "Unspecified platform application interface."
#endif
#endif // GEAROENIX_SYSTEM_APPLICATION_HPP
