#include "gx-cr-application.hpp"
#include "../render/engine/gx-rnd-eng-engine.hpp"
#include "../system/gx-sys-application.hpp"

gearoenix::core::Application::Application(system::Application* const sys_app) noexcept
    : system_application(sys_app)
    , render_engine(sys_app->get_render_engine())
{
}

gearoenix::core::Application::~Application() noexcept
{
    system_application = nullptr;
    render_engine = nullptr;
}

void gearoenix::core::Application::update() noexcept
{
}

void gearoenix::core::Application::terminate() noexcept
{
}
