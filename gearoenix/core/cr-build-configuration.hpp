#ifndef GEAROENIX_CORE_BUILD_CONFIGURATION_HPP
#define GEAROENIX_CORE_BUILD_CONFIGURATION_HPP
/// Platform definition
/// --------------------------------------------------------------------------
#ifdef ANDROID
#define IN_ANDROID
//#define USE_VULKAN
//#define USE_OPENGL_ES3
#define USE_OPENGL_ES2
#elif defined(__unix__)
#define IN_LINUX
//#define USE_VULKAN
//#define USE_OPENGL_41
//#define USE_OPENGL_33
//#define USE_OPENGL_ES3
#define USE_OPENGL_ES2
#elif defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64)
#define IN_WINDOWS
//#define USE_VULKAN
//#define USE_OPENGL_41
//#define USE_OPENGL_33
//#define USE_OPENGL_ES3
#define USE_OPENGL_ES2
#elif defined(__APPLE__)
#include "TargetConditionals.h"
#if defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_IPHONE)
#define IN_IOS
//#define USE_METAL
//#define USE_VULKAN
//#define USE_OPENGL_ES3
#define USE_OPENGL_ES2
#elif defined(TARGET_OS_MAC)
#define IN_MAC
//#define USE_METAL
//#define USE_VULKAN
//#define USE_OPENGL_41
//#define USE_OPENGL_33
#else
#error "Unknown Apple platform"
#endif
#else
#error "Not implemetned yet."
#endif
#if defined(IN_WINDOWS) || defined(IN_LINUX) || defined(IN_MAC)
#define IN_DESKTOP
#endif
#define USE_SDL
//#define USE_GLFW
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
#define DEFAULT_WINDOW_WIDTH 800
#define DEFAULT_WINDOW_HEIGHT 600
#endif
#endif
