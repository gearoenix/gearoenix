#ifndef GEAROENIX_PLATFORM_ANDROID_MAIN_ENTRY_HPP
#define GEAROENIX_PLATFORM_ANDROID_MAIN_ENTRY_HPP
#include "../gx-plt-build-configuration.hpp"
#ifdef GX_PLATFORM_INTERFACE_ANDROID
#include <android_native_app_glue.h>

#define GX_MAIN_ENTRY_ARGS_DEF [[maybe_unused]] android_app* state

#define GX_MAIN_ENTRY_ARGS state

#define GX_MAIN_ENTRY(expr)                   \
    void android_main(GX_MAIN_ENTRY_ARGS_DEF) \
    {                                         \
        expr;                                 \
    }

#endif
#endif