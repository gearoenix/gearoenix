#include "gx-cr-application.hpp"
#include "../platform/gx-plt-application.hpp"
#include "../render/engine/gx-rnd-eng-engine.hpp"

gearoenix::core::Application::Application(platform::Application* const sys_app) noexcept
    : platform_application(sys_app)
    , render_engine(sys_app->get_render_engine())
{
}

gearoenix::core::Application::~Application() noexcept
{
    platform_application = nullptr;
    render_engine = nullptr;
}

void gearoenix::core::Application::update() noexcept
{
}

void gearoenix::core::Application::terminate() noexcept
{
}
