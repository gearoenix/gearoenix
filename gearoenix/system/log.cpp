#include "sys-log.hpp"
#ifndef IN_ANDROID
std::ofstream gearoenix::system::Log::info("info.txt");
std::ofstream gearoenix::system::Log::debug("debug.txt");
std::ofstream gearoenix::system::Log::error("error.txt");
#endif
