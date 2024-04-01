#ifndef GEAROENIX_PLATFORM_BILLING_HPP
#define GEAROENIX_PLATFORM_BILLING_HPP
#include "gx-plt-bll-build-configuration.hpp"
#include <functional>
#include <string>

#define GX_PLT_BILLING_FUNCTIONS_MAP(gx_plt_billing_functions_map_arg) \
    gx_plt_billing_functions_map_arg(acknowledged);                    \
    gx_plt_billing_functions_map_arg(consumed);                        \
    gx_plt_billing_functions_map_arg(pending);                         \
    gx_plt_billing_functions_map_arg(purchased);                       \
    gx_plt_billing_functions_map_arg(unpurchased)

namespace gearoenix::platform {
struct Billing final {
    static std::function<void(int)> on_acknowledged;
    static std::function<void(int)> on_consumed;
    static std::function<void(int)> on_pending;
    static std::function<void(int)> on_purchased;
    static std::function<void(int)> on_unpurchased;

public:
    static void launch_flow(int product_index);
    // TODO: provide an API to provide the product info from the native code
    static void query_products_info();
    static void query_purchases_info();
};
}

#endif