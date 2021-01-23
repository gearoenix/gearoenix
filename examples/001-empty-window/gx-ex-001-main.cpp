#include <gearoenix/platform/gx-plt-application.hpp>
#include <gearoenix/platform/gx-plt-log.hpp>

void start(GX_MAIN_ENTRY_ARGS_DEF) noexcept
{
    auto plt_app = gearoenix::platform::Application(GX_MAIN_ENTRY_ARGS);
    plt_app.run();
    GX_LOG_D("Ended.")
}

GX_MAIN_ENTRY(start(GX_MAIN_ENTRY_ARGS))