#ifndef GEAROENIX_CORE_BUILD_CONFIGURATION_HPP
#define GEAROENIX_CORE_BUILD_CONFIGURATION_HPP
/// Platform definition
/// --------------------------------------------------------------------------
#ifdef ANDROID
#define IN_ANDROID
//#define USE_VULKAN
//#define USE_OPENGL_ES3
#define USE_OPENGL_ES2
#elif defined(__EMSCRIPTEN__)
#define IN_WEB
//#define USE_OPENGL_ES3
#define USE_OPENGL_ES2
#define USE_SDL
#elif defined(__unix__)
#define IN_LINUX
//#define USE_VULKAN
//#define USE_OPENGL_43
//#define USE_OPENGL_33
//#define USE_OPENGL_ES3
#define USE_OPENGL_ES2
#define IN_DESKTOP
#define USE_SDL
//#define USE_GLFW
#elif defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64)
#define IN_WINDOWS
//#define USE_VULKAN
//#define USE_DIRECTX12
#define USE_DIRECTX11
//#define USE_OPENGL_43
//#define USE_OPENGL_33
#define IN_DESKTOP
//#define USE_SDL
//#define USE_GLFW
#define USE_WINAPI
#elif defined(__APPLE__)
#include "TargetConditionals.h"
#if TARGET_OS_IPHONE
#define IN_IOS
//#define USE_METAL
//#define USE_VULKAN
//#define USE_OPENGL_ES3
#define USE_OPENGL_ES2
#define USE_SDL
#elif TARGET_OS_MAC
#define IN_MAC
//#define USE_METAL
//#define USE_VULKAN
//#define USE_OPENGL_43
#define USE_OPENGL_33
#define USE_OPENGL_ES3
#define IN_DESKTOP
#define USE_SDL
//#define USE_GLFW
#else
#error "Unknown Apple platform"
#endif // IOS MAC
#else
#error "Not implemetned yet."
#endif
/// Application constant definitions
/// -------------------------------------------------------------
#define APPLICATION_NAME "Gearoenix Demo App"
/// Application compilation mode
/// -----------------------------------------------------------------
#define DEBUG_MODE
/// Application features
/// -------------------------------------------------------------------------
#ifdef DEBUG_MODE
#define LOG_ENABLED
#define GEAROENIX_DEFAULT_WINDOW_WIDTH 800
#define GEAROENIX_DEFAULT_WINDOW_HEIGHT 600
#else
#define GEAROENIX_FULLSCREEN
#endif
#define GEAROENIX_FULLSCREEN
#ifndef IN_WEB
#define THREAD_SUPPORTED
#endif
#define PROFILING_MODE
#endif
