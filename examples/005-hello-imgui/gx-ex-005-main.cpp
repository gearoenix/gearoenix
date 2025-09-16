#include <gearoenix/core/gx-cr-application.hpp>
#include <gearoenix/platform/gx-plt-file-chooser.hpp>

#include <ImGui/imgui.h>

struct GameApp final : gearoenix::core::Application {
private:
    bool show_demo_window = true;
    bool show_hello_window = true;
    bool show_file_browser_window = true;

    std::string opened_file_name = {};
    bool file_browser_canceled = false;

public:
    GameApp() = default;

    void update() override
    {
        ImGui::ShowDemoWindow(&show_demo_window);

        if (ImGui::Begin("Gearoenix File Browser Operations", &show_file_browser_window)) {
            ImGui::Text("This window contains a demo for showing how the file browser API can be used");
            ImGui::Text("The file browser API is implemented in Gearoenix and have a uniform API across all platforms.");

            if (!opened_file_name.empty()) {
                ImGui::Text("Opened file name: %s", opened_file_name.c_str());
            }

            if (file_browser_canceled) {
                ImGui::Text("File browser canceled");
            }

            if (ImGui::Button("Save")) {
                opened_file_name.clear();
                file_browser_canceled = false;

                gearoenix::platform::file_chooser_save("gearoenix-hello.txt", "[Gearoenix Game Engine] Save a Text File", ".txt", { 'H', 'e', 'l', 'l', 'o', '!', '\0' });
            }

            if (ImGui::Button("Open")) {
                opened_file_name.clear();
                file_browser_canceled = false;

                gearoenix::platform::file_chooser_open([this](auto&& name, auto&&) {
                    file_browser_canceled = false;
                    opened_file_name = std::move(name); }, [this] { file_browser_canceled = true; }, "[Gearoenix Game Engine] Open Any File", ".*");
            }
        } else {
            opened_file_name.clear();
            file_browser_canceled = false;
        }
        ImGui::End();
    }
};

GEAROENIX_START(GameApp)