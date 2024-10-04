#include "gx-plt-bll-billing.hpp"

#define GX_DEF_STATIC_CALLBACKS(f) decltype(gearoenix::platform::Billing::on_##f) gearoenix::platform::Billing::on_##f = [](auto) { }

GX_PLT_BILLING_FUNCTIONS_MAP(GX_DEF_STATIC_CALLBACKS);

#if GX_BILLING_NONE

void gearoenix::platform::Billing::launch_flow(int)
{
}

void gearoenix::platform::Billing::query_products_info()
{
}

void gearoenix::platform::Billing::query_purchases_info()
{
}

#endif