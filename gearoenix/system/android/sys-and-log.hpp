#ifndef GEAROENIX_SYSTEM_ANDROID_LOG_HPP
#define GEAROENIX_SYSTEM_ANDROID_LOG_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef IN_ANDROID
#include <android/log.h>
#include <sstream>

#define GXLOG(p, s)                                                                 \
    {                                                                               \
        std::stringstream stringstream;                                             \
        stringstream << __FILE__ << " " << __LINE__ << ": ";                        \
        stringstream << s;                                                          \
        __android_log_print(p, APPLICATION_NAME, "%s", stringstream.str().c_str()); \
    }

#define GXLOGI(s) GXLOG(ANDROID_LOG_INFO, s)
#define GXLOGE(s) GXLOG(ANDROID_LOG_ERROR, s)
#define GXLOGF(s)                \
    GXLOG(ANDROID_LOG_FATAL, s); \
    std::terminate();

#endif
#endif
