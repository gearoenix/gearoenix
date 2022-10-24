#ifndef GEAROENIX_PLATFORM_BUILD_CONFIGURATION_HPP
#define GEAROENIX_PLATFORM_BUILD_CONFIGURATION_HPP

#ifdef ANDROID

#define GX_PLATFORM_ANDROID
// #define GX_PLATFORM_INTERFACE_SDL2
#define GX_PLATFORM_INTERFACE_ANDROID

#elif defined(__EMSCRIPTEN__)

#define GX_PLATFORM_WEBASSEMBLY
#define GX_PLATFORM_INTERFACE_SDL2

#elif defined(__unix__)

#define GX_PLATFORM_LINUX
#define GX_PLATFORM_INTERFACE_SDL2
#ifndef GX_PLATFORM_INTERFACE_SDL2
#define GX_PLATFORM_INTERFACE_X11
#endif
#define GX_PLATFORM_DESKTOP

#elif defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64)

#define NOMINMAX
#define GX_PLATFORM_WINDOWS
#define GX_PLATFORM_INTERFACE_SDL2
#ifndef GX_PLATFORM_INTERFACE_SDL2
#define GX_PLATFORM_INTERFACE_WIN32
#endif
#define GX_PLATFORM_DESKTOP

#elif defined(__APPLE__)

#include "TargetConditionals.h"
#if TARGET_OS_IPHONE
#define GX_PLATFORM_IOS
#elif TARGET_OS_MAC
#define GX_PLATFORM_MACOS
#define GX_PLATFORM_DESKTOP
#else
#error "Unknown Apple platform"
#endif

#else
#error "Not implemetned yet."
#endif

#endif