#ifndef GEAROENIX_PLATFORM_LOG_HPP
#define GEAROENIX_PLATFORM_LOG_HPP
#include "../core/gx-cr-build-configuration.hpp"
#include "../core/macro/gx-cr-mcr-concatenate.hpp"
#include "gx-plt-build-configuration.hpp"
#include "macro/gx-plt-mcr-lock.hpp"
#include <ctime>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <mutex>
#include <sstream>
#include <thread>

#define GX_PLT_LOG_SS_VAR GX_CONCAT_5(_gearoenix_platform_log_ss_, __LINE__)
#define GX_PLT_LOG_TIME GX_CONCAT_5(_gearoenix_platform_log_time_, __LINE__)
#define GX_PLT_LOG_LOCAL GX_CONCAT_5(_gearoenix_platform_log_local_, __LINE__)

#ifdef GX_DEBUG_MODE
#define GX_PLT_LOG_FILE_LOCK_GUARD GX_GUARD_LOCK(gearoenix::platform::Log::file)
#define GX_PLT_LOG_END_OF_MSG << std::flush
#else
#define GX_PLT_LOG_FILE_LOCK_GUARD
#define GX_PLT_LOG_END_OF_MSG
#endif

#define GX_PLT_LOG_COMMON_STR(msg, msg_type)                                                      \
    std::stringstream GX_PLT_LOG_SS_VAR;                                                          \
    auto GX_PLT_LOG_TIME = std::time(nullptr);                                                    \
    std::tm GX_PLT_LOG_LOCAL {};                                                                  \
    localtime_s(&GX_PLT_LOG_LOCAL, &GX_PLT_LOG_TIME);                                             \
    GX_PLT_LOG_SS_VAR << std::put_time(&GX_PLT_LOG_LOCAL, "%d-%m-%Y %H-%M-%S")                    \
                      << " [thread:" << std::this_thread::get_id() << "]-["                       \
                      << __FILE__ << ":" << __LINE__ << "]-[" << msg_type << "] " << msg << "\n"; \
    GX_PLT_LOG_FILE_LOCK_GUARD

#define GX_PLT_LOG_COMMON(msg, msg_type)                                             \
    GX_PLT_LOG_COMMON_STR(msg, msg_type)                                             \
    gearoenix::platform::Log::file << GX_PLT_LOG_SS_VAR.str() GX_PLT_LOG_END_OF_MSG; \
    std::cout << GX_PLT_LOG_SS_VAR.str() GX_PLT_LOG_END_OF_MSG;

#define GX_LOG_I(s)                  \
    {                                \
        GX_PLT_LOG_COMMON(s, "INFO") \
    }
#ifdef GX_DEBUG_MODE
#define GX_LOG_D(s)                   \
    {                                 \
        GX_PLT_LOG_COMMON(s, "DEBUG") \
    }
#else
#define GX_LOG_D(s)
#endif
#define GX_LOG_E(s)                   \
    {                                 \
        GX_PLT_LOG_COMMON(s, "ERROR") \
    }
#define GX_LOG_F(s)                   \
    {                                 \
        GX_PLT_LOG_COMMON(s, "FATAL") \
        std::terminate();             \
    }

#ifdef GX_PLT_ANDROID
#include "android/gx-sys-and-log.hpp"
#elif defined(GX_PLATFORM_WINDOWS)
#include "windows/gx-plt-win-log.hpp"
#endif

#ifndef GX_PLT_ANDROID
namespace gearoenix::platform {
struct Log {
    static std::ofstream file;
    GX_CREATE_GUARD_S(file)
};
}
#endif

#define GX_REACHED GX_LOG_D("REACHED <-- <-- <-- <-- <-- <-- <-- <--")
#define GX_TODO GX_LOG_E("TODO")
#define GX_UNEXPECTED GX_LOG_F("Unexpected")
#define GX_UNIMPLEMENTED GX_LOG_F("Unimplemented")

#endif