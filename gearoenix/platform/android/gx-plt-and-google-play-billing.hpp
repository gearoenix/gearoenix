#ifndef GEAROENIX_PLATFORM_ANDROID_GOOGLE_PLAY_BILLING_HPP
#define GEAROENIX_PLATFORM_ANDROID_GOOGLE_PLAY_BILLING_HPP
#include "../gx-plt-build-configuration.hpp"
#ifdef GX_PLATFORM_INTERFACE_ANDROID
#include <jni.h>

namespace gearoenix::platform {
    struct GooglePlayBilling final {
        static jclass billing_java_class;
        static jmethodID launch_billing_flow_java_method;

    private:

    public:
        static void initialise_static(JNIEnv *env) noexcept;
        static void launch_flow(const char* product_id) noexcept;
    };
}

#endif
#endif