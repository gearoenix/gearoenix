#ifndef GEAROENIX_NUFRAG_SYSTEM_APPLICATION_HPP
#define GEAROENIX_NUFRAG_SYSTEM_APPLICATION_HPP
#include "../core/build-configuration.hpp"
#ifdef IN_ANDROID
#include "android/and-app.hpp"
#elif defined(IN_LINUX)
#include "linux/linux-app.hpp"
#else
#include "windows/win-app.hpp"
#endif // IN_ANDROID
#endif // GEAROENIX_NUFRAG_SYSTEM_APPLICATION_HPP
