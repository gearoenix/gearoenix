#ifndef GEAROENIX_SYSTEM_APPLICATION_HPP
#define GEAROENIX_SYSTEM_APPLICATION_HPP
#include "../core/cr-build-configuration.hpp"
#ifdef USE_SDL
#include "sdl/sys-sdl-app.hpp"
#elif defined(USE_GLFW)
#include "glfw/sys-glfw.hpp"
#else
#error "Unimplemented"
#endif // IN_ANDROID
#endif // GEAROENIX_SYSTEM_APPLICATION_HPP
