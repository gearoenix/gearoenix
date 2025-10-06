#pragma once

#ifdef ANDROID

#define GX_PLATFORM_ANDROID true
// #define GX_PLATFORM_INTERFACE_SDL2 true
#define GX_PLATFORM_INTERFACE_ANDROID true

#elif defined(__EMSCRIPTEN__)

#define GX_PLATFORM_WEBASSEMBLY true
#define GX_PLATFORM_INTERFACE_SDL2 true

#elif defined(__unix__)

#define GX_PLATFORM_LINUX true
#define GX_PLATFORM_INTERFACE_SDL2 true
#if !GX_PLATFORM_INTERFACE_SDL2
#define GX_PLATFORM_INTERFACE_X11 true
#endif
#define GX_PLATFORM_DESKTOP true

#elif defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64)

#ifdef NOMINMAX
#undef NOMINMAX
#endif
#define NOMINMAX true
#define GX_PLATFORM_WINDOWS true
#define GX_PLATFORM_INTERFACE_SDL2 true
#if !GX_PLATFORM_INTERFACE_SDL2
#define GX_PLATFORM_INTERFACE_WIN32 true
#endif
#define GX_PLATFORM_DESKTOP true

#elif defined(__APPLE__)

#define GX_PLATFORM_APPLE true
#define GX_PLATFORM_INTERFACE_SDL2 true
#if !GX_PLATFORM_INTERFACE_SDL2
#define GX_PLATFORM_INTERFACE_APPKIT true
#endif
#include "TargetConditionals.h"
#if TARGET_OS_IPHONE
#define GX_PLATFORM_IOS true
#elif TARGET_OS_MAC
#define GX_PLATFORM_MACOS true
#define GX_PLATFORM_DESKTOP true
#else
#error "Unknown Apple platform"
#endif

#else
#error "Not implemetned yet."
#endif