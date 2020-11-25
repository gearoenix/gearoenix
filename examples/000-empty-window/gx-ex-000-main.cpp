#include <gearoenix/platform/gx-plt-application.hpp>
#include <gearoenix/platform/gx-plt-log.hpp>
#include <gearoenix/platform/gx-plt-main-entry.hpp>

GX_MAIN_ENTRY(
    gearoenix::platform::Application().run();
    GX_LOG_F("Ended!");)