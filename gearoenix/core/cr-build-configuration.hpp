#ifndef GEAROENIX_CORE_BUILD_CONFIGURATION_HPP
#define GEAROENIX_CORE_BUILD_CONFIGURATION_HPP
// Platform definition
#ifdef ANDROID
#define GX_IN_ANDROID
//#define GX_USE_VULKAN
//#define GX_USE_OPENGL_ES3
#define GX_USE_OPENGL_ES2
#elif defined(__EMSCRIPTEN__)
#define GX_IN_WEB
//#define GX_USE_OPENGL_ES3
#define GX_USE_OPENGL_ES2
#define GX_USE_SDL
#elif defined(__unix__)
#define GX_IN_LINUX
//#define GX_USE_VULKAN
//#define GX_USE_OPENGL_43
//#define GX_USE_OPENGL_33
//#define GX_USE_OPENGL_ES3
#define GX_USE_OPENGL_ES2
#define GX_IN_DESKTOP
#define GX_USE_SDL
//#define GX_USE_GLFW
#elif defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64)
#define GX_IN_WINDOWS
//#define GX_USE_VULKAN
//#define GX_USE_DIRECTX12
//#define GX_USE_DIRECTX11
//#define GX_USE_OPENGL_43
//#define GX_USE_OPENGL_33
//#define GX_USE_OPENGL_ES3
#define GX_USE_OPENGL_ES2
#define GX_IN_DESKTOP
#define GX_USE_SDL
//#define GX_USE_GLFW
//#define GX_USE_WINAPI
#elif defined(__APPLE__)
#include "TargetConditionals.h"
#if TARGET_OS_IPHONE
#define GX_IN_IOS
//#define GX_USE_METAL
//#define GX_USE_VULKAN
//#define GX_USE_OPENGL_ES3
#define GX_USE_OPENGL_ES2
#define GX_USE_SDL
#elif TARGET_OS_MAC
#define GX_IN_MAC
//#define GX_USE_METAL
//#define GX_USE_VULKAN
//#define GX_USE_OPENGL_43
#define GX_USE_OPENGL_33
#define GX_USE_OPENGL_ES3
#define GX_IN_DESKTOP
#define GX_USE_SDL
//#define GX_USE_GLFW
#else
#error "Unknown Apple platform"
#endif // IOS MAC
#else
#error "Not implemetned yet."
#endif
// Application constant definitions
#define GX_APP_NAME "Gearoenix Demo App"
// Application compilation mode
#define GX_DEBUG_MODE
//#define TEST_MODE
#define GX_GAME_MODE
// Application features

#define GX_LOG_ENABLED

#ifdef GX_LOG_ENABLED
#define GX_LOG_ERROR_ENABLED
#define GX_LOG_WARN_ENABLED
#define GX_LOG_INFO_ENABLED
#endif

// You can comment this for windowd apps
#define GX_FULLSCREEN
// Window aspect in debug mode
#ifndef GX_FULLSCREEN
#define GX_DEFAULT_WINDOW_WIDTH 1000
#define GX_DEFAULT_WINDOW_HEIGHT 700
#endif
#define GX_PROFILING_MODE

#define GX_MAX_DIRECTIONAL_LIGHTS 8
#define GX_MAX_POINT_LIGHTS 8
#define GX_MAX_CONE_LIGHTS 8
#define GX_MAX_SHADOW_MAPS 6

#endif
