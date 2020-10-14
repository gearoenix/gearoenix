#ifndef GEAROENIX_SYSTEM_BUILD_CONFIGURATION_HPP
#define GEAROENIX_SYSTEM_BUILD_CONFIGURATION_HPP
#ifdef ANDROID
#define GX_IN_ANDROID
#elif defined(__EMSCRIPTEN__)
#define GX_IN_WEB
#define GX_THREAD_NOT_SUPPORTED
#elif defined(__unix__)
#define GX_IN_LINUX
#define GX_IN_DESKTOP
#elif defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64)
#define NOMINMAX
#define GX_IN_WINDOWS
#define GX_IN_DESKTOP
#elif defined(__APPLE__)
#include "TargetConditionals.h"
#if TARGET_OS_IPHONE
#define GX_IN_IOS
#elif TARGET_OS_MAC
#define GX_IN_MAC
#define GX_IN_DESKTOP
#else
#error "Unknown Apple platform"
#endif
#else
#error "Not implemetned yet."
#endif
#endif