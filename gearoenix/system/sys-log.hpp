#ifndef GEAROENIX_SYSTEM_LOG_HPP
#define GEAROENIX_SYSTEM_LOG_HPP
#include "../core/cr-build-configuration.hpp"
#ifdef LOG_ENABLED
#include <exception>
#ifdef IN_ANDROID
#include "android/sys-and-log.hpp"
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
#define GXLOGI(s)                                                       \
    gearoenix::system::Log::info << APPLICATION_NAME << " " << s << " " \
                                 << __FILE__ << " " << __LINE__ << std::endl;
#define GXLOGD(s)                                                        \
    gearoenix::system::Log::debug << APPLICATION_NAME << " " << s << " " \
                                  << __FILE__ << " " << __LINE__ << std::endl;
#define GXLOGE(s)                                                        \
    gearoenix::system::Log::error << APPLICATION_NAME << " " << s << " " \
                                  << __FILE__ << " " << __LINE__ << std::endl;
#define GXLOGF(s) \
    GXLOGE(s);    \
    std::terminate();
#endif // IN_ANDROID
#define TODO GXLOGE("TODO")
#define UNEXPECTED GXLOGF("Unexpected")
#define UNIMPLEMENTED GXLOGF("Unimplemented")
#else // LOG_ENABLED
#endif // LOG_ENABLED
#endif // GEAROENIX_SYSTEM_LOG_HPP
