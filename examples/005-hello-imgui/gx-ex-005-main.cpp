#include <gearoenix/core/gx-cr-application.hpp>
#include <imgui.h>

struct GameApp final : gearoenix::core::Application {
private:
    bool show_demo_window = true;
    bool show_hello_window = true;

public:
    GameApp() = default;

    void update() override
    {
        Application::update();

        if (show_demo_window) {
            ImGui::ShowDemoWindow(&show_demo_window);
        }

        if (show_hello_window) {
            if (ImGui::Begin("Hello Window!", &show_hello_window)) {
                ImGui::Text("Hello Label!");
                if (ImGui::Button("Close")) {
                    show_hello_window = false;
                }
            }
            ImGui::End();
        }
    }
};

GEAROENIX_START(GameApp)