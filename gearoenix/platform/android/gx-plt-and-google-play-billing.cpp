#include "gx-plt-and-google-play-billing.hpp"
#ifdef GX_PLATFORM_INTERFACE_ANDROID
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "gx-plt-and-jni.hpp"

jclass gearoenix::platform::GooglePlayBilling::billing_java_class = nullptr;
jmethodID gearoenix::platform::GooglePlayBilling::launch_billing_flow_java_method = nullptr;

void gearoenix::platform::GooglePlayBilling::initialise_static(JNIEnv *const env) noexcept {
    billing_java_class = env->FindClass("com/geareonix/billing/GooglePlay");

    if(nullptr == billing_java_class || JNI_TRUE == env->ExceptionCheck()) {
        if(JNI_TRUE == env->ExceptionCheck()) {
            env->ExceptionClear();
        }
        return;
    }

    launch_billing_flow_java_method = env->GetStaticMethodID(
            billing_java_class, "launchBillingFlow", "(Ljava/lang/String;)V");
}

void gearoenix::platform::GooglePlayBilling::launch_flow(const char *const product_id) noexcept {
    GX_ASSERT_D(nullptr != billing_java_class);
    GX_ASSERT_D(nullptr != launch_billing_flow_java_method);

    JniUtility::make_current();
    JniUtility::env->CallStaticVoidMethod(
            billing_java_class,
            launch_billing_flow_java_method,
            JniUtility::env->NewStringUTF(product_id));
}

#endif
