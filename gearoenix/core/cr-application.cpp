#include "cr-application.hpp"

gearoenix::core::Application::Application(const std::shared_ptr<system::Application> &sys_app)
    : sys_app(sys_app)
{
}

gearoenix::core::Application::~Application() {}
