#ifndef GEAROENIX_SYSTEM_LOG_HPP
#define GEAROENIX_SYSTEM_LOG_HPP
#include "../core/cr-build-configuration.hpp"
#ifdef LOG_ENABLED
#include <exception>
#ifdef IN_ANDROID
#include "android/and-log.hpp"
#else
#include <fstream>
namespace gearoenix {
namespace system {
    class Log {
    public:
        static std::ofstream info;
        static std::ofstream debug;
        static std::ofstream error;
    };
}
}
#define LOGI(s)                                                         \
    gearoenix::system::Log::info << APPLICATION_NAME << " " << s << " " \
                                 << __FILE__ << " " << __LINE__ << std::endl;
#define LOGD(s)                                                          \
    gearoenix::system::Log::debug << APPLICATION_NAME << " " << s << " " \
                                  << __FILE__ << " " << __LINE__ << std::endl;
#define LOGE(s)                                                          \
    gearoenix::system::Log::error << APPLICATION_NAME << " " << s << " " \
                                  << __FILE__ << " " << __LINE__ << std::endl;
#define LOGF(s) \
    LOGE(s);    \
    std::terminate();
#endif // IN_ANDROID
#define TODO LOGE("TODO")
#define UNEXPECTED LOGF("Unexpected")
#define UNIMPLEMENTED LOGF("Unimplemented")
#else // LOG_ENABLED
#endif // LOG_ENABLED
#endif // GEAROENIX_SYSTEM_LOG_HPP
