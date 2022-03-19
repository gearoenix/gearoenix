#ifndef GEAROENIX_PLATFORM_LOG_HPP
#define GEAROENIX_PLATFORM_LOG_HPP
#include "../core/gx-cr-build-configuration.hpp"
#include "../core/macro/gx-cr-mcr-concatenate.hpp"
#include "gx-plt-build-configuration.hpp"
#include "macro/gx-plt-mcr-lock.hpp"
#include <exception>
#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>

// You can uses it for enabling log print in the std out
//#define GX_PLATFORM_LOG_STD_OUT_ENABLED

// You can uses it for appending to the log file
//#define GX_PLATFORM_LOG_APPEND_FILE

namespace gearoenix::platform {
struct Log {
#ifndef GX_PLATFORM_ANDROID
    static std::ofstream file;
#endif
    GX_CREATE_GUARD_S(log);
    static std::stringstream header(const char*, int, const char*);
};
}

#define GX_PLT_LOG_SS_VAR GX_CONCAT_5(_gearoenix_platform_log_ss_, __LINE__)
#define GX_PLT_LOG_STR_VAR GX_CONCAT_5(_gearoenix_platform_log_str_, __LINE__)

#ifdef GX_DEBUG_MODE
#define GX_PLT_LOG_FILE_LOCK_GUARD GX_GUARD_LOCK(gearoenix::platform::Log::log);
#define GX_PLT_LOG_END_OF_MSG << std::flush
#else
#define GX_PLT_LOG_FILE_LOCK_GUARD
#define GX_PLT_LOG_END_OF_MSG
#endif

#define GX_PLT_LOG_COMMON_STR(msg, msg_type)                                                              \
    std::stringstream GX_PLT_LOG_SS_VAR = gearoenix::platform::Log::header(__FILE__, __LINE__, msg_type); \
    GX_PLT_LOG_SS_VAR << msg << "\n";                                                                     \
    const auto GX_PLT_LOG_STR_VAR = GX_PLT_LOG_SS_VAR.str();                                              \
    GX_PLT_LOG_FILE_LOCK_GUARD

#ifdef GX_PLATFORM_LOG_STD_OUT_ENABLED
#define GX_PLT_LOG_STD_PRINT std::cout << GX_PLT_LOG_STR_VAR GX_PLT_LOG_END_OF_MSG;
#else
#define GX_PLT_LOG_STD_PRINT
#endif

#define GX_PLT_LOG_COMMON(msg, msg_type)                                        \
    GX_PLT_LOG_COMMON_STR(msg, msg_type)                                        \
    gearoenix::platform::Log::file << GX_PLT_LOG_STR_VAR GX_PLT_LOG_END_OF_MSG; \
    GX_PLT_LOG_STD_PRINT

#define GX_LOG_I(s)                  \
    {                                \
        GX_PLT_LOG_COMMON(s, "INFO") \
    }                                \
    static_assert(true, "")
#ifdef GX_DEBUG_MODE
#define GX_LOG_D(s)                   \
    {                                 \
        GX_PLT_LOG_COMMON(s, "DEBUG") \
    }                                 \
    static_assert(true, "")
#else
#define GX_LOG_D(s)
#endif
#define GX_LOG_E(s)                   \
    {                                 \
        GX_PLT_LOG_COMMON(s, "ERROR") \
    }                                 \
    static_assert(true, "")
#define GX_LOG_F(s)                   \
    {                                 \
        GX_PLT_LOG_COMMON(s, "FATAL") \
        std::terminate();             \
    }                                 \
    static_assert(true, "")

#ifdef GX_PLATFORM_INTERFACE_ANDROID
#include "android/gx-plt-and-log.hpp"
#elif defined(GX_PLATFORM_INTERFACE_WIN32)
#include "win32/gx-plt-win-log.hpp"
#endif

#define GX_REACHED GX_LOG_D("REACHED <-- <-- <-- <-- <-- <-- <-- <--")
#define GX_TODO GX_LOG_E("TODO")
#define GX_UNEXPECTED GX_LOG_F("Unexpected")
#define GX_UNIMPLEMENTED GX_LOG_F("Unimplemented")

#endif