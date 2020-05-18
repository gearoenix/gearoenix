#include "sys-log.hpp"
#if !defined(GX_IN_ANDROID) && !defined(GX_IN_WEB) && defined(GX_LOG_ENABLED) && !defined(GX_IN_IOS)
std::ofstream gearoenix::system::Log::info("info.txt");
GX_CREATE_GUARD(gearoenix::system::Log::info)
#ifdef GX_DEBUG_MODE
std::ofstream gearoenix::system::Log::debug("debug.txt");
GX_CREATE_GUARD(gearoenix::system::Log::debug)
#endif
std::ofstream gearoenix::system::Log::error("error.txt");
GX_CREATE_GUARD(gearoenix::system::Log::error)
#endif
