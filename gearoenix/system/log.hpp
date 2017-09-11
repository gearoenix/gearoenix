#ifndef GEAROENIX_NUFRAG_SYSTEM_LOG_HPP
#define GEAROENIX_NUFRAG_SYSTEM_LOG_HPP
#include "../core/build-configuration.hpp"
#ifdef LOG_ENABLED
#include <exception>
#ifdef IN_ANDROID
#include "android/and-log.hpp"
#else
#include <fstream>
namespace gearoenix {
namespace nufrag {
namespace system {
class Log {
public:
	static std::ofstream info;
	static std::ofstream debug;
	static std::ofstream error;
};
}
}
}
#define LOGI(s) gearoenix::nufrag::system::Log::info << APPLICATION_NAME << " " << s << " " << __FILE__ << " " << __LINE__ << std::endl;
#define LOGD(s) gearoenix::nufrag::system::Log::debug << APPLICATION_NAME << " " << s << " " << __FILE__ << " " << __LINE__ << std::endl;
#define LOGE(s) gearoenix::nufrag::system::Log::error << APPLICATION_NAME << " " << s << " " << __FILE__ << " " << __LINE__ << std::endl;
#define LOGF(s) LOGE(s); std::terminate();
#endif // IN_ANDROID
#else // LOG_ENABLED
#endif // LOG_ENABLED
#endif // GEAROENIX_NUFRAG_SYSTEM_LOG_HPP
