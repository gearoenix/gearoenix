#ifndef GEAROENIX_PLATFORM_BILLING_BUILD_CONFIGURATION_HPP
#define GEAROENIX_PLATFORM_BILLING_BUILD_CONFIGURATION_HPP
#include "../gx-plt-build-configuration.hpp"

#ifdef GX_PLATFORM_ANDROID
#define GX_BILLING_GOOGLE_PLAY true

#elif GX_PLATFORM_WEBASSEMBLY

#define GX_BILLING_NONE true

#elif GX_PLATFORM_LINUX

#define GX_BILLING_NONE true
// #define GX_BILLING_STEAM

#elif GX_PLATFORM_WINDOWS

#define GX_BILLING_NONE true
// #define GX_BILLING_MS_STORE true
// #define GX_BILLING_STEAM true
// #define GX_BILLING_EPIC true

#elif GX_PLATFORM_IOS

#define GX_BILLING_APP_STORE true

#elif GX_PLATFORM_MACOS

#define GX_BILLING_APP_STORE true
// #define GX_BILLING_STEAM true
// #define GX_BILLING_EPIC true

#else

#error "Unknown platform"

#endif

#endif