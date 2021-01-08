#include "gx-plt-lnx-application.hpp"

#ifdef GX_PLATFORM_LINUX

gearoenix::platform::Application::Application(const RuntimeConfiguration& config) noexcept
{
}

std::shared_ptr<gearoenix::platform::Application> gearoenix::platform::Application::construct(
    const RuntimeConfiguration& config) noexcept
{
    return std::shared_ptr<Application>();
}

#endif