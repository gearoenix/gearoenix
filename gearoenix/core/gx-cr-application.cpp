#include "gx-cr-application.hpp"

gearoenix::core::Application::Application()
    : Singleton(this)
{
}

gearoenix::core::Application::~Application() = default;

void gearoenix::core::Application::update()
{
}