#ifndef GEAROENIX_SYSTEM_LOG_HPP
#define GEAROENIX_SYSTEM_LOG_HPP
#include "../core/cr-build-configuration.hpp"
#ifdef GX_LOG_ENABLED
#include <exception>
#ifdef GX_IN_ANDROID
#include "android/sys-and-log.hpp"
#elif defined(GX_IN_WEB) || defined(GX_IN_IOS)
#include <iostream>
#define GXLOGI(s)                                                  \
    std::cout << GX_APP_NAME << " " << s << " " << __FILE__ << " " \
              << __LINE__ << std::endl;
#ifdef GX_DEBUG_MODE
#define GXLOGD(s) GXLOGI(s)
#else
#define GXLOGD(s)
#endif
#define GXLOGE(s)                                                         \
    std::cout << GX_APP_NAME << " ERROR: " << s << " " << __FILE__ << " " \
              << __LINE__ << std::endl;
#define GXLOGF(s)         \
    {                     \
        GXLOGE(s);        \
        std::terminate(); \
    }
#else
#include <fstream>
#include <mutex>
namespace gearoenix {
namespace system {
    class Log {
    public:
        static std::ofstream info;
        static std::mutex info_lock;
#ifdef GX_DEBUG_MODE
        static std::ofstream debug;
        static std::mutex debug_lock;
#endif
        static std::ofstream error;
        static std::mutex error_lock;
    };
}
}
#define GXLOGI(s)                                                                 \
    {                                                                             \
        std::lock_guard<std::mutex> _lg(gearoenix::system::Log::info_lock);       \
        gearoenix::system::Log::info << GX_APP_NAME << " " << s << " "            \
                                     << __FILE__ << " " << __LINE__ << std::endl; \
    }
#ifdef GX_DEBUG_MODE
#define GXLOGD(s)                                                                  \
    {                                                                              \
        std::lock_guard<std::mutex> _lg(gearoenix::system::Log::debug_lock);       \
        gearoenix::system::Log::debug << GX_APP_NAME << " " << s << " "            \
                                      << __FILE__ << " " << __LINE__ << std::endl; \
    }
#endif
#define GXLOGE(s)                                                                  \
    {                                                                              \
        std::lock_guard<std::mutex> _lg(gearoenix::system::Log::error_lock);       \
        gearoenix::system::Log::error << GX_APP_NAME << " " << s << " "            \
                                      << __FILE__ << " " << __LINE__ << std::endl; \
    }
#define GXLOGF(s)         \
    {                     \
        GXLOGE(s);        \
        std::terminate(); \
    }
#endif // IN_ANDROID
#define GXREACHED GXLOGD("REACHED-------------------------------------------")
#define GXTODO GXLOGE("TODO")
#define GXUNEXPECTED GXLOGF("Unexpected")
#define GXUNIMPLEMENTED GXLOGF("Unimplemented")
#define GXASSERT(x) \
    if ((x) == 0)   \
    GXUNEXPECTED
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
