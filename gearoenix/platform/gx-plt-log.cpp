#include "gx-plt-log.hpp"
#include <chrono>
#include <iomanip>
#include <thread>

#ifdef GX_PLATFORM_LOG_APPEND_FILE
#define GX_PLT_LOG_FILE_APPEND | std::ios::app
#else
#define GX_PLT_LOG_FILE_APPEND
#endif

#ifndef GX_PLT_ANDROID
std::ofstream gearoenix::platform::Log::file(GX_APPLICATION_NAME ".log", std::ios::out GX_PLT_LOG_FILE_APPEND);
#endif

GX_CREATE_GUARD(gearoenix::platform::Log::log);

std::stringstream gearoenix::platform::Log::header(const char* const file_name, int line, const char* msg_type)
{
    std::stringstream s;
    auto t = std::time(nullptr);
    std::tm tm {};
#ifdef GX_PLATFORM_WINDOWS
    (void)localtime_s(&tm, &t);
#else
    tm = *std::localtime(&t);
#endif
    s << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "."
      << (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count() % 1000000000)
      << " [thread:" << std::this_thread::get_id() << "]-[" << file_name << ":" << line << "]-[" << msg_type << "] ";
    return s;
}
