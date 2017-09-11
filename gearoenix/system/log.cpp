#include "log.hpp"
#ifndef IN_ANDROID 
std::ofstream gearoenix::nufrag::system::Log::info("info.txt");
std::ofstream gearoenix::nufrag::system::Log::debug("debug.txt");
std::ofstream gearoenix::nufrag::system::Log::error("error.txt");
#endif