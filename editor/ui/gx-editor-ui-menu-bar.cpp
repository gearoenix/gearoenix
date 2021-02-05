#include "gx-editor-ui-menu-bar.hpp"
#include "../control/gx-editor-ctrl-manager.hpp"
#include <gearoenix/platform/gx-plt-log.hpp>
#include <gearoenix/platform/gx-plt-application.hpp>
#include <imgui.h>

void gearoenix::editor::ui::MenuBar::show_project() noexcept {
    if (ImGui::BeginMenu("Project"))
    {
        auto& project_ctrl = control_manager->get_project();
        if (ImGui::MenuItem("New", "Ctrl+N"))
            show_new_project_popup = true;

        if (ImGui::MenuItem("Open", "Ctrl+O")) {}

        const auto& recent_files = project_ctrl.get_recent_file_addresses();
        if (ImGui::BeginMenu("Open Recent", !recent_files.empty()))
        {
            for(const auto& f: recent_files) {
                ImGui::MenuItem(f.c_str());
            }
            ImGui::EndMenu();
        }

        const auto& core_project = project_ctrl.get_project();
        if (ImGui::MenuItem("Save", "Ctrl+S", false, core_project.has_value())) {}
        if (ImGui::MenuItem("Save As..", "Ctrl+Shift+S", false, core_project.has_value())) {}
        if (ImGui::MenuItem("Settings", "Ctrl+Alt+P", false, core_project.has_value())) {}
        if (ImGui::MenuItem("Quit", "Ctrl+Shift+Q")) {}
        ImGui::EndMenu();
    }



    if(show_new_project_popup) {
        ImGui::OpenPopup("New Project");
        ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        if (ImGui::BeginPopupModal("New Project", &show_new_project_popup, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("New project name:");
//
//            // Testing behavior of widgets stacking their own regular popups over the modal.
//            static int item = 1;
//            static float color[4] = { 0.4f, 0.7f, 0.0f, 0.5f };
//            ImGui::Combo("Combo", &item, "aaaa\0bbbb\0cccc\0dddd\0eeee\0\0");
//            ImGui::ColorEdit4("color", color);
//
//            if (ImGui::Button("Add another modal.."))
//                ImGui::OpenPopup("Stacked 2");
//
//            // Also demonstrate passing a bool* to BeginPopupModal(), this will create a regular close button which
//            // will close the popup. Note that the visibility state of popups is owned by imgui, so the input value
//            // of the bool actually doesn't matter here.
//            bool unused_open = true;
//            if (ImGui::BeginPopupModal("Stacked 2", &unused_open))
//            {
//                ImGui::Text("Hello from Stacked The Second!");
//                if (ImGui::Button("Close"))
//                    ImGui::CloseCurrentPopup();
//                ImGui::EndPopup();
//            }

            if (ImGui::Button("Close")) {
                show_new_project_popup = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
}

gearoenix::editor::ui::MenuBar::MenuBar(
        platform::Application * const platform_application,
        editor::control::Manager *const control_manager) noexcept
    : platform_application(platform_application)
    , control_manager(control_manager)
{
}

void gearoenix::editor::ui::MenuBar::update() noexcept {
    if (ImGui::BeginMainMenuBar())
    {

        show_project();
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
            const bool is_fullscreen = platform_application->get_base().get_configuration().get_fullscreen();
            if (ImGui::MenuItem(is_fullscreen? "Unset Fullscreen": "Set Fullscreen", "F11")) {
                platform_application->set_window_fullscreen(!is_fullscreen);
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}
