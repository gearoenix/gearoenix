#ifndef GEAROENIX_PLATFORM_BUILD_CONFIGURATION_HPP
#define GEAROENIX_PLATFORM_BUILD_CONFIGURATION_HPP
#ifdef ANDROID
#define GX_PLT_ANDROID
#elif defined(__EMSCRIPTEN__)
#define GX_PLT_WEB
#define GX_PLT_THREAD_NOT_SUPPORTED
#elif defined(__unix__)
#define GX_PLT_LINUX
#define GX_PLT_DESKTOP
#elif defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64)
#define NOMINMAX
#define GX_PLT_WINAPI
#define GX_PLT_DESKTOP
#elif defined(__APPLE__)
#include "TargetConditionals.h"
#if TARGET_OS_IPHONE
#define GX_PLT_IOS
#elif TARGET_OS_MAC
#define GX_PLT_MAC
#define GX_PLT_DESKTOP
#else
#error "Unknown Apple platform"
#endif
#else
#error "Not implemetned yet."
#endif
#endif