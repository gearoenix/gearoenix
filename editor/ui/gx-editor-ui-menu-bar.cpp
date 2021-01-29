#include "gx-editor-ui-menu-bar.hpp"
#include <imgui.h>

void gearoenix::editor::ui::MenuBar::update() noexcept {
    if (ImGui::BeginMainMenuBar())
    {

        if (ImGui::BeginMenu("Project"))
        {
            if (ImGui::MenuItem("New", "Ctrl+N")) {}
            if (ImGui::MenuItem("Open", "Ctrl+O")) {}
            if (ImGui::BeginMenu("Open Recent"))
            {
                ImGui::MenuItem("Forest.gx3d");
                ImGui::MenuItem("Gear.gx3d");
                ImGui::MenuItem("Phoenix.gx3d");
                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Save", "Ctrl+S")) {}
            if (ImGui::MenuItem("Save As..", "Ctrl+Shift+S")) {}
            if (ImGui::MenuItem("Settings", "Ctrl+Alt+P")) {}
            if (ImGui::MenuItem("Quit", "Ctrl+Shift+Q")) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Scene"))
        {
            if (ImGui::MenuItem("New", "Ctrl+Shift+N", false, false)) {}
            if (ImGui::MenuItem("Delete", "Ctrl+Shift+Del", false, false)) {}
            if (ImGui::MenuItem("Settings", "Ctrl+Alt+S", false, false)) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Object"))
        {
            if (ImGui::MenuItem("New", "Alt+O,Alt+N", false, false)) {}
            if (ImGui::MenuItem("Delete", "Alt+O,Alt+Del", false, false)) {}
            if (ImGui::MenuItem("Settings", "Ctrl+Alt+O", false, false)) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Audio"))
        {
            if (ImGui::MenuItem("Settings", "Ctrl+Alt+A", false, false)) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Window"))
        {
            if (ImGui::MenuItem("Fullscreen", "F11")) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}
