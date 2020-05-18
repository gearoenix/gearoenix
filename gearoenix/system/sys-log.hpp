#ifndef GEAROENIX_SYSTEM_LOG_HPP
#define GEAROENIX_SYSTEM_LOG_HPP
#include "../core/cr-static.hpp"
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
namespace gearoenix::system {
class Log {
public:
    static std::ofstream info;
    GX_CREATE_GUARD_S(info);
#ifdef GX_DEBUG_MODE
    static std::ofstream debug;
    GX_CREATE_GUARD_S(debug)
#endif
    static std::ofstream error;
    GX_CREATE_GUARD_S(error)
};
}
#define GXLOGI(s)                                                                 \
    {                                                                             \
        GX_GUARD_LOCK(gearoenix::system::Log::info)                               \
        gearoenix::system::Log::info << GX_APP_NAME << " " << s << " "            \
                                     << __FILE__ << " " << __LINE__ << std::endl; \
    }
#ifdef GX_DEBUG_MODE
#define GXLOGD(s)                                                                  \
    {                                                                              \
        GX_GUARD_LOCK(gearoenix::system::Log::debug)                               \
        gearoenix::system::Log::debug << GX_APP_NAME << " " << s << " "            \
                                      << __FILE__ << " " << __LINE__ << std::endl; \
    }
#endif
#define GXLOGE(s)                                                                  \
    {                                                                              \
        GX_GUARD_LOCK(gearoenix::system::Log::error)                               \
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
