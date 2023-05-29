#ifndef GEAROENIX_PLATFORM_ANDROID_JNI_HPP
#define GEAROENIX_PLATFORM_ANDROID_JNI_HPP
#include "../gx-plt-build-configuration.hpp"
#ifdef GX_PLATFORM_INTERFACE_ANDROID
#include <jni.h>

namespace gearoenix::platform {
    struct JniUtility final {
        static JavaVM* jvm;
        static JNIEnv* env;

        static void make_current() noexcept;
    };
}

#endif
#endif