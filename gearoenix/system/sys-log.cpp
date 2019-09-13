#include "sys-log.hpp"
#if !defined(GX_IN_ANDROID) && !defined(GX_IN_WEB) && defined(GX_LOG_ENABLED)
std::ofstream gearoenix::system::Log::info("info.txt");
#ifdef GX_DEBUG_MODE
std::ofstream gearoenix::system::Log::debug("debug.txt");
std::mutex gearoenix::system::Log::debug_lock;
#endif
std::ofstream gearoenix::system::Log::error("error.txt");
#endif
