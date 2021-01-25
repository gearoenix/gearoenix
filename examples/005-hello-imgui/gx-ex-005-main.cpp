#include <gearoenix/core/gx-cr-application.hpp>
#include <gearoenix/platform/gx-plt-log.hpp>

struct GameApp final : public gearoenix::core::Application {
    explicit GameApp(gearoenix::platform::Application* plt_app) noexcept;
    void update() noexcept final;
};

GameApp::GameApp(gearoenix::platform::Application* plt_app) noexcept
    : Application(plt_app)
{
}

void GameApp::update() noexcept
{
    Application::update();
}

GEAROENIX_START(GameApp)