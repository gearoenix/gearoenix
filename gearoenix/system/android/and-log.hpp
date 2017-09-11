#ifndef GEAROENIX_NUFRAG_SYSTEM_ANDROID_LOG_HPP
#define GEAROENIX_NUFRAG_SYSTEM_ANDROID_LOG_HPP
#include "../../core/build-configuration.hpp"
#ifdef IN_ANDROID
#include <android/log.h>
#define LOGI(s) __android_log_print(ANDROID_LOG_INFO, APPLICATION_NAME, "%s ----- in file: %s %d", (s).c_str(), __FILE__, __LINE__);
#define LOGD(s) __android_log_print(ANDROID_LOG_DEBUG, APPLICATION_NAME, "%s ----- in file: %s %d", (s).c_str(), __FILE__, __LINE__);
#define LOGE(s) __android_log_print(ANDROID_LOG_ERROR, APPLICATION_NAME, "%s ----- in file: %s %d", (s).c_str(), __FILE__, __LINE__);
#define LOGF(s) __android_log_print(ANDROID_LOG_FATAL, APPLICATION_NAME, "%s ----- in file: %s %d", (s).c_str(), __FILE__, __LINE__); std::terminate();
#endif
#endif
