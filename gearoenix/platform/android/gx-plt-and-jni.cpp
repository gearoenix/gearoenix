#include "gx-plt-and-jni.hpp"
#ifdef GX_PLATFORM_INTERFACE_ANDROID
#include <jni.h>
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "gx-plt-and-google-play-billing.hpp"

JavaVM* gearoenix::platform::JniUtility::jvm = nullptr;
JNIEnv* gearoenix::platform::JniUtility::env = nullptr;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *const jvm, void *const) {
    gearoenix::platform::JniUtility::jvm = jvm;

    if (jvm->GetEnv((void**) &gearoenix::platform::JniUtility::env, JNI_VERSION_1_2) != JNI_OK) {
        return JNI_ERR;
    }

    gearoenix::platform::GooglePlayBilling::initialise_static(gearoenix::platform::JniUtility::env);

    return JNI_VERSION_1_2;
}

void gearoenix::platform::JniUtility::make_current() noexcept {
    GX_ASSERT(JNI_ERR != jvm->AttachCurrentThread(&env, nullptr));
}

#endif