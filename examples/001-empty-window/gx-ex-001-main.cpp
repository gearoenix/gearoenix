#include <gearoenix/platform/gx-plt-application.hpp>
#include <gearoenix/platform/gx-plt-log.hpp>
#include <gearoenix/platform/gx-plt-main-entry.hpp>

void start() noexcept
{
    const auto plt_app = gearoenix::platform::Application::construct();
    plt_app->run();
    GX_LOG_D("Ended.")
}

GX_MAIN_ENTRY(start())