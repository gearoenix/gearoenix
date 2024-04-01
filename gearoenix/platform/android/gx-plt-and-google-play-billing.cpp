#include "gx-plt-and-google-play-billing.hpp"
#ifdef GX_PLATFORM_INTERFACE_ANDROID
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../billing/gx-plt-bll-billing.hpp"
#include "gx-plt-and-jni.hpp"

jclass gearoenix::platform::GooglePlayBilling::billing_java_class = nullptr;
jmethodID gearoenix::platform::GooglePlayBilling::launch_billing_flow_java_method = nullptr;
jmethodID gearoenix::platform::GooglePlayBilling::query_product_details_async_static_method = nullptr;
jmethodID gearoenix::platform::GooglePlayBilling::query_product_purchases_async_static_method = nullptr;

#define GX_DEC_FUNCTIONS(f)                                                                           \
    extern "C" JNIEXPORT void JNICALL Java_com_geareonix_billing_GooglePlay_00024Native_product_1##f( \
        JNIEnv* const, jclass, jint product_index)                                                    \
    {                                                                                                 \
        if (product_index < 0)                                                                        \
            return;                                                                                   \
        gearoenix::platform::Billing::on_##f(static_cast<int>(product_index));                        \
    }                                                                                                 \
    static_assert(true, "")

GX_PLT_BILLING_FUNCTIONS_MAP(GX_DEC_FUNCTIONS);
#undef GX_HELPER

void gearoenix::platform::GooglePlayBilling::initialise_static(JNIEnv* const env)
{
    // prevent optimisation the function alive
#define GX_KEEP_FUNCTION_ALIVE(f) Java_com_geareonix_billing_GooglePlay_00024Native_product_1##f(nullptr, nullptr, -1)
#define GX_KEEP_FUNCTIONS_ALIVE GX_PLT_BILLING_FUNCTIONS_MAP(GX_KEEP_FUNCTION_ALIVE)

    GX_KEEP_FUNCTIONS_ALIVE;

    billing_java_class = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("com/geareonix/billing/GooglePlay")));

    if (nullptr == billing_java_class) {
        return;
    }

    if (JNI_TRUE == env->ExceptionCheck()) {
        env->ExceptionClear();
        return;
    }

    launch_billing_flow_java_method = env->GetStaticMethodID(
        billing_java_class, "launchBillingFlow", "(I)V");
    GX_ASSERT_D(nullptr != GooglePlayBilling::launch_billing_flow_java_method);

    query_product_details_async_static_method = env->GetStaticMethodID(
        billing_java_class, "queryProductDetailsAsyncStatic", "()V");
    GX_ASSERT_D(nullptr != GooglePlayBilling::query_product_details_async_static_method);

    query_product_purchases_async_static_method = env->GetStaticMethodID(
        billing_java_class, "queryProductPurchasesAsyncStatic", "()V");
    GX_ASSERT_D(nullptr != GooglePlayBilling::query_product_purchases_async_static_method);
}

void gearoenix::platform::Billing::launch_flow(const int product_index)
{
    GX_KEEP_FUNCTIONS_ALIVE;

    GX_ASSERT_D(nullptr != GooglePlayBilling::billing_java_class);

    JniUtility::make_current();
    JniUtility::env->CallStaticVoidMethod(
        GooglePlayBilling::billing_java_class,
        GooglePlayBilling::launch_billing_flow_java_method,
        static_cast<jint>(product_index));
}

void gearoenix::platform::Billing::query_products_info()
{
    GX_KEEP_FUNCTIONS_ALIVE;

    GX_ASSERT_D(nullptr != GooglePlayBilling::billing_java_class);

    JniUtility::make_current();
    JniUtility::env->CallStaticVoidMethod(
        GooglePlayBilling::billing_java_class,
        GooglePlayBilling::query_product_details_async_static_method);
}

void gearoenix::platform::Billing::query_purchases_info()
{
    GX_KEEP_FUNCTIONS_ALIVE;

    GX_ASSERT_D(nullptr != GooglePlayBilling::billing_java_class);

    JniUtility::make_current();
    JniUtility::env->CallStaticVoidMethod(
        GooglePlayBilling::billing_java_class,
        GooglePlayBilling::query_product_purchases_async_static_method);
}

#undef GX_HELPER

#endif