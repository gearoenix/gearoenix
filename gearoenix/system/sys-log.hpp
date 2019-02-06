#ifndef GEAROENIX_SYSTEM_LOG_HPP
#define GEAROENIX_SYSTEM_LOG_HPP
#include "../core/cr-build-configuration.hpp"
#ifdef GX_LOG_ENABLED
#include <exception>
#ifdef GX_IN_ANDROID
#include "android/sys-and-log.hpp"
#elif defined(GX_IN_WEB)
#include <iostream>
#define GXLOGI(s)                                                       \
    std::cout << APPLICATION_NAME << " " << s << " " << __FILE__ << " " \
              << __LINE__ << std::endl;
#define GXLOGD(s) GXLOGI(s)
#define GXLOGE(s)                                                              \
    std::cout << APPLICATION_NAME << " ERROR: " << s << " " << __FILE__ << " " \
              << __LINE__ << std::endl;
#define GXLOGF(s)         \
    {                     \
        GXLOGE(s);        \
        std::terminate(); \
    }
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
#define GXLOGI(s)                                                                 \
    {                                                                             \
        gearoenix::system::Log::info << GX_APP_NAME << " " << s << " "       \
                                     << __FILE__ << " " << __LINE__ << std::endl; \
    }
#define GXLOGD(s)                                                                  \
    {                                                                              \
        gearoenix::system::Log::debug << GX_APP_NAME << " " << s << " "       \
                                      << __FILE__ << " " << __LINE__ << std::endl; \
    }
#define GXLOGE(s)                                                                  \
    {                                                                              \
        gearoenix::system::Log::error << GX_APP_NAME << " " << s << " "       \
                                      << __FILE__ << " " << __LINE__ << std::endl; \
    }
#define GXLOGF(s)         \
    {                     \
        GXLOGE(s);        \
        std::terminate(); \
    }
#endif // IN_ANDROID
#define GXTODO GXLOGE("TODO")
#define GXUNEXPECTED GXLOGF("Unexpected")
#define GXUNIMPLEMENTED GXLOGF("Unimplemented")
#define GXASSERT(x) \
    if ((x) == 0)   \
    GXUNEXPECTED
#define GXREACHED GXLOGI("REACHED-------------------------------------------")
#else // GX_LOG_ENABLED
#define GXLOGI(s) ;
#define GXLOGD(s) ;
#define GXLOGE(s) ;
#define GXLOGF(s) std::terminate();
#define GXTODO ;
#define GXUNEXPECTED std::terminate();
#define GXUNIMPLEMENTED std::terminate();
#endif // LOG_ENABLED
#endif // GEAROENIX_SYSTEM_LOG_HPP
