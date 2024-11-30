#pragma once
#include "../gx-plt-build-configuration.hpp"
#ifdef GX_PLATFORM_INTERFACE_ANDROID
#include <jni.h>
#include <string>

namespace gearoenix::platform {
struct JniUtility final {
    static JavaVM* jvm;
    static JNIEnv* env;

    static void make_current();
    [[nodiscard]] static std::string consume_string(jstring j_str);
};
}

#endif