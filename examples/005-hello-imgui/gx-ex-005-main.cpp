#include <gearoenix/core/gx-cr-application.hpp>
#include <gearoenix/platform/gx-plt-log.hpp>
#include <imgui.h>

struct GameApp final : public gearoenix::core::Application {
    explicit GameApp(gearoenix::platform::Application& plt_app) noexcept;
    void update() noexcept final;
};

GameApp::GameApp(gearoenix::platform::Application& plt_app) noexcept
    : Application(plt_app)
{
}

void GameApp::update() noexcept
{
    static bool show_window = true;
    Application::update();

    ImGui::ShowDemoWindow(&show_window);

    ImGui::Begin("Hello Window!", &show_window);
    ImGui::Text("Hello Label!");
    if (ImGui::Button("Close"))
        show_window = false;
    ImGui::End();
}

GEAROENIX_START(GameApp)