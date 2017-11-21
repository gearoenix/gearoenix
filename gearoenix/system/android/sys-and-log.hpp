#ifndef GEAROENIX_SYSTEM_ANDROID_LOG_HPP
#define GEAROENIX_SYSTEM_ANDROID_LOG_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef IN_ANDROID
#include <sstream>
#include <android/log.h>

void gearoenix_android_log_print(
  android_LogPriority prio, const std::stringstream& str)
{
  __android_log_print(prio, APPLICATION_NAME, "%s", str.str().c_str());
}

#define LOG(p, s) { \
  std::stringstream stringstream; \
  stringstream << __FILE__ << " " << __LINE__ << ":"; \
  stringstream << s; \
  gearoenix_android_log_print(p, stringstream); \
}

#define LOGI(s) LOG(ANDROID_LOG_INFO, s)
#define LOGE(s) LOG(ANDROID_LOG_ERROR, s)
#define LOGF(s) LOG(ANDROID_LOG_FATAL, s); std::terminate();

#endif
#endif
