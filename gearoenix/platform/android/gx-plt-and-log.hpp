#ifndef GEAROENIX_PLATFORM_ANDROID_LOG_HPP
#define GEAROENIX_PLATFORM_ANDROID_LOG_HPP
#include "../gx-plt-build-configuration.hpp"
#ifdef GX_PLATFORM_ANDROID
#include <android/log.h>

#ifdef GX_LOG_I
#undef GX_LOG_I
#endif

#ifdef GX_LOG_D
#undef GX_LOG_D
#endif

#ifdef GX_LOG_E
#undef GX_LOG_E
#endif

#ifdef GX_LOG_F
#undef GX_LOG_F
#endif

#define GX_LOG_I(s)                                                                                   \
    {                                                                                                 \
        GX_PLT_LOG_COMMON_STR(s, "INFO")                                                              \
        __android_log_print(ANDROID_LOG_INFO, GX_APPLICATION_NAME, "%s", GX_PLT_LOG_STR_VAR.c_str()); \
    }                                                                                                 \
    static_assert(true, "")
#ifdef GX_DEBUG_MODE

#define GX_LOG_D(s)                                                                                    \
    {                                                                                                  \
        GX_PLT_LOG_COMMON_STR(s, "DEBUG")                                                              \
        __android_log_print(ANDROID_LOG_DEBUG, GX_APPLICATION_NAME, "%s", GX_PLT_LOG_STR_VAR.c_str()); \
    }                                                                                                  \
    static_assert(true, "")
#else
#define GX_LOG_D(s)
#endif

#define GX_LOG_E(s)                                                                                    \
    {                                                                                                  \
        GX_PLT_LOG_COMMON_STR(s, "ERROR")                                                              \
        __android_log_print(ANDROID_LOG_ERROR, GX_APPLICATION_NAME, "%s", GX_PLT_LOG_STR_VAR.c_str()); \
    }                                                                                                  \
    static_assert(true, "")

#define GX_LOG_F(s)                                                                                    \
    {                                                                                                  \
        GX_PLT_LOG_COMMON_STR(s, "FATAL")                                                              \
        __android_log_print(ANDROID_LOG_FATAL, GX_APPLICATION_NAME, "%s", GX_PLT_LOG_STR_VAR.c_str()); \
        std::terminate();                                                                              \
    }                                                                                                  \
    static_assert(true, "")

#endif
#endif
