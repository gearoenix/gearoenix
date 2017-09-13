#ifndef GEAROENIX_SYSTEM_APPLICATION_HPP
#define GEAROENIX_SYSTEM_APPLICATION_HPP
#include "../core/cr-build-configuration.hpp"
#ifdef IN_ANDROID
#include "android/and-app.hpp"
#elif defined(IN_LINUX)
#include "linux/sys-linux-app.hpp"
#else
#include "windows/win-app.hpp"
#endif // IN_ANDROID
#endif // GEAROENIX_SYSTEM_APPLICATION_HPP
