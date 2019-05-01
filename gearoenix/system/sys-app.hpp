#ifndef GEAROENIX_SYSTEM_APPLICATION_HPP
#define GEAROENIX_SYSTEM_APPLICATION_HPP
#include "../core/cr-build-configuration.hpp"
#ifdef GX_USE_SDL
#include "sdl/sys-sdl-app.hpp"
#elif defined(GX_USE_GLFW)
#include "glfw/sys-glfw.hpp"
#elif defined(GX_IN_ANDROID)
#include "android/sys-and-app.hpp"
#elif defined(GX_USE_WINAPI)
#include "win/sys-win-app.hpp"
#else
#error "Unspecified platform application interface."
#endif
#endif // GEAROENIX_SYSTEM_APPLICATION_HPP
