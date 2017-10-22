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
//#define USE_OPENGL_4
//#define USE_OPENGL_ES3
#define USE_OPENGL_ES2
#elif defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64)
#define IN_WINDOWS
//#define USE_VULKAN
//#define USE_OPENGL_4
//#define USE_OPENGL_ES3
#define USE_OPENGL_ES2
#else
#error "Not implemetned yet."
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
