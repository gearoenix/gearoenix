#include "gx-plt-log.hpp"
#if !defined(GX_PLT_ANDROID) && !defined(GX_PLT_WEB) && !defined(GX_PLT_IOS)
std::ofstream gearoenix::platform::Log::file(GX_APPLICATION_NAME ".log", std::ios::out | std::ios::app);
GX_CREATE_GUARD(gearoenix::platform::Log::file)
#endif
