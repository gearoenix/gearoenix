#ifndef GEAROENIX_SYSTEM_APPLICATION_HPP
#define GEAROENIX_SYSTEM_APPLICATION_HPP
#include "../core/cr-build-configuration.hpp"
#ifdef USE_SDL
#include "sdl/sys-sdl-app.hpp"
#elif defined(USE_GLFW)
#include "glfw/sys-glfw.hpp"
#elif defined(IN_ANDROID)
#include "android/sys-and-app.hpp"
#else
#error "Unimplemented"
#endif
#endif // GEAROENIX_SYSTEM_APPLICATION_HPP
