#include <gearoenix/core/gx-cr-application.hpp>
#include <gearoenix/platform/gx-plt-file-chooser.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>

#include <ImGui/imgui.h>

struct GameApp final : gearoenix::core::Application {
private:
    std::string opened_file_name = {};
    bool file_browser_canceled = false;

public:
    GameApp() = default;

    void update() override
    {
        ImGui::ShowDemoWindow();

        if (ImGui::Begin("Gearoenix File Browser Operations")) {
            ImGui::Text("This window contains a demo for showing how the file browser API can be used");
            ImGui::Text("The file browser API is implemented in Gearoenix and have a uniform API across all platforms.");

            if (file_browser_canceled) {
                ImGui::Text("File browser canceled");
            } else if (!opened_file_name.empty()) {
                ImGui::Text("Opened file name: %s", opened_file_name.c_str());
            }

            if (ImGui::Button("Save")) {
                opened_file_name.clear();
                file_browser_canceled = false;

                gearoenix::platform::file_chooser_save("gearoenix-hello.txt", "[Gearoenix Game Engine] Save a Text File", ".txt", { 'H', 'e', 'l', 'l', 'o', '!', '\0' }, [] { });
            }

            if (ImGui::Button("Open")) {
                opened_file_name.clear();
                file_browser_canceled = false;

                gearoenix::platform::file_chooser_open([this](auto&& path, auto&&) {
                    file_browser_canceled = false;
                    opened_file_name = path.get_raw_data(); }, [this] {
                    file_browser_canceled = true;
                    opened_file_name.clear(); }, "[Gearoenix Game Engine] Open Any File", ".*");
            }
        } else {
            opened_file_name.clear();
            file_browser_canceled = false;
        }
        ImGui::End();
    }
};

GEAROENIX_START(GameApp)