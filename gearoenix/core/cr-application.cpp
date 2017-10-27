#include "cr-application.hpp"

gearoenix::core::Application::Application(system::Application* sys_app)
    : sys_app(sys_app)
{
}

gearoenix::core::Application::~Application() {}

void gearoenix::core::Application::on_zoom(Real) {}

void gearoenix::core::Application::on_rotate(Real) {}
