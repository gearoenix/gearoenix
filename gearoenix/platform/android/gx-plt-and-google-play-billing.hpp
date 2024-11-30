#pragma once
#include "../gx-plt-build-configuration.hpp"
#ifdef GX_PLATFORM_INTERFACE_ANDROID
#include <jni.h>

namespace gearoenix::platform {
struct GooglePlayBilling final {
    static jclass billing_java_class;

    static jmethodID launch_billing_flow_java_method;
    static jmethodID query_product_details_async_static_method;
    static jmethodID query_product_purchases_async_static_method;

private:
public:
    static void initialise_static(JNIEnv* env);
};
}

#endif