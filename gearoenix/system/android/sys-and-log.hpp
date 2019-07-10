#ifndef GEAROENIX_SYSTEM_ANDROID_LOG_HPP
#define GEAROENIX_SYSTEM_ANDROID_LOG_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_IN_ANDROID
#include <android/log.h>
#include <sstream>

#define GXLOG(p, s)                                                                 \
    {                                                                               \
        std::stringstream stringstream;                                             \
        stringstream << __FILE__ << " " << __LINE__ << ": ";                        \
        stringstream << s;                                                          \
        __android_log_print(p, GX_APP_NAME, "%s", stringstream.str().c_str()); \
    }

#define GXLOGI(s) GXLOG(ANDROID_LOG_INFO, s)
#ifdef GX_DEBUG_MODE
#define GXLOGD(s) GXLOG(ANDROID_LOG_DEBUG, s)
#else
#define GXLOGD(s)
#endif
#define GXLOGE(s) GXLOG(ANDROID_LOG_ERROR, s)
#define GXLOGF(s)                    \
    {                                \
        GXLOG(ANDROID_LOG_FATAL, s); \
        std::terminate();            \
    }

#endif
#endif
