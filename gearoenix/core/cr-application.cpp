#include "cr-application.hpp"

gearoenix::core::Application::Application(system::Application* sys_app)
    : sys_app(sys_app)
{
}

gearoenix::core::Application::~Application() {}

void gearoenix::core::Application::on_zoom(Real) {}

void gearoenix::core::Application::on_rotate(Real) {}

void gearoenix::core::Application::on_scroll(Real) {}

void gearoenix::core::Application::on_mouse(MouseButton, ButtonAction, Real, Real) {}

void gearoenix::core::Application::on_mouse_move(Real, Real) {}
