#include "gx-plt-and-jni.hpp"
#ifdef GX_PLATFORM_INTERFACE_ANDROID
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../billing/gx-plt-bll-build-configuration.hpp"
#include "gx-plt-and-google-play-billing.hpp"
#include <jni.h>

JavaVM* gearoenix::platform::JniUtility::jvm = nullptr;
JNIEnv* gearoenix::platform::JniUtility::env = nullptr;

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* const jvm, void* const)
{
    if (jvm == nullptr)
        return JNI_ERR;

    gearoenix::platform::JniUtility::jvm = jvm;

    if (jvm->GetEnv((void**)&gearoenix::platform::JniUtility::env, JNI_VERSION_1_2) != JNI_OK) {
        return JNI_ERR;
    }

#if GX_BILLING_GOOGLE_PLAY
    gearoenix::platform::GooglePlayBilling::initialise_static(gearoenix::platform::JniUtility::env);
#endif

    return JNI_VERSION_1_2;
}

#define GX_KEEP_FUNCTIONS_ALIVE (void)JNI_OnLoad(nullptr, nullptr)

void gearoenix::platform::JniUtility::make_current()
{
    GX_KEEP_FUNCTIONS_ALIVE;

    GX_ASSERT(JNI_ERR != jvm->AttachCurrentThread(&env, nullptr));
}

std::string gearoenix::platform::JniUtility::consume_string(jstring j_str)
{
    GX_KEEP_FUNCTIONS_ALIVE;

    const char* c_str = env->GetStringUTFChars(j_str, nullptr);
    std::string str(c_str);
    env->ReleaseStringUTFChars(j_str, c_str);
    env->DeleteLocalRef(j_str);
    return str;
}

#endif