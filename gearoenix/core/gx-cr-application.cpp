#include "gx-cr-application.hpp"
#include "../platform/gx-plt-application.hpp"
#include "../render/engine/gx-rnd-eng-engine.hpp"

gearoenix::core::Application::Application(platform::Application* const platform_application) noexcept
    : platform_application(platform_application)
    , render_engine(platform_application->get_base().get_render_engine())
{
}

gearoenix::core::Application::~Application() noexcept = default;

void gearoenix::core::Application::update() noexcept
{
}