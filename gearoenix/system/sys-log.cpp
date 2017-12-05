#include "sys-log.hpp"
#if !defined(IN_ANDROID) && !defined(IN_WEB)
std::ofstream gearoenix::system::Log::info("info.txt");
std::ofstream gearoenix::system::Log::debug("debug.txt");
std::ofstream gearoenix::system::Log::error("error.txt");
#endif
