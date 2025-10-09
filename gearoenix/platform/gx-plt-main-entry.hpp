#pragma once
#include "gx-plt-build-configuration.hpp"

#ifdef GX_PLATFORM_INTERFACE_WIN32
#include "win32/gx-plt-win-main-entry.hpp"
#elif defined(GX_PLATFORM_INTERFACE_SDL)
#include "sdl/gx-plt-sdl-main-entry.hpp"
#elif defined(GX_PLATFORM_INTERFACE_ANDROID)
#include "android/gx-plt-and-main-entry.hpp"
#else

#define GX_MAIN_ENTRY_ARGS_DEF \
    [[maybe_unused]] int argc, \
        [[maybe_unused]] char const *const *argv

#define GX_MAIN_ENTRY_ARGS argc, argv

#define GX_MAIN_ENTRY(expr)          \
    int main(GX_MAIN_ENTRY_ARGS_DEF) \
    {                                \
        expr;                        \
        return 0;                    \
    }

#endif