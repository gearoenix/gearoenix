#ifndef GEAROENIX_CORE_BUILD_CONFIGURATION_HPP
#define GEAROENIX_CORE_BUILD_CONFIGURATION_HPP
/// Platform definition --------------------------------------------------------------------------
#ifdef ANDROID
#define IN_ANDROID
#elif defined(__unix__)
#define IN_LINUX
#elif defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64)
#define IN_WINDOWS
#endif
/// Graphic API choosing -------------------------------------------------------------------------
#define USE_VULKAN
/// Application constant definitions -------------------------------------------------------------
#define APPLICATION_NAME "Gearoenix Demo App"
/// Application compilation mode -----------------------------------------------------------------
#define DEBUG_MODE
/// Application features -------------------------------------------------------------------------
#ifdef DEBUG_MODE
#define LOG_ENABLED
#define DEFAULT_WINDOW_WIDTH 800
#define DEFAULT_WINDOW_HEIGHT 600
#endif
#endif
