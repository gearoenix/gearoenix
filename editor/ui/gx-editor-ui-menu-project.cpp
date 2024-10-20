#include "gx-editor-ui-menu-project.hpp"
#include <gearoenix/platform/gx-plt-runtime-configuration.hpp>
#include <gearoenix/render/gx-rnd-runtime-configuration.hpp>
#include <imgui/imgui.h>

#include "gearoenix/platform/sdl2/gx-plt-sdl2-application.hpp"
#include "gearoenix/render/engine/gx-rnd-eng-engine.hpp"
#include "gx-editor-ui-manager.hpp"

void gearoenix::editor::ui::MenuProject::show_new_popup()
{
    if (!is_new_popup_open) {
        return;
    }
    ImGui::OpenPopup("New Project");
    const ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("New Project", &is_new_popup_open, ImGuiWindowFlags_AlwaysAutoResize)) {
        // ImGui::InputText("New project name", &project.get_project_name());
        // if (!project.get_project_name().empty()) {
        //     if (ImGui::Button("Create")) {
        // TODO check for existence of a current unsaved project
        //         project.create_project();
        //         show_project_new_popup = false;
        //         ImGui::CloseCurrentPopup();
        //     }
        // }
        ImGui::EndPopup();
    }
}

void gearoenix::editor::ui::MenuProject::show_settings()
{
    if (!is_settings_open) {
        return;
    }

    if (!ImGui::Begin("Project Settings", &is_settings_open)) {
        ImGui::End();
        return;
    }

    auto* const w = manager.get_platform_application().get_base().get_render_engine()->get_world();

    if (ImGui::TreeNode("Platform Settings")) {
        platform::RuntimeConfiguration::get(manager.get_platform_application()).show_debug_gui(*w);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Render Settings")) {
        render::RuntimeConfiguration::get(w).show_debug_gui(*w);
        ImGui::TreePop();
    }

    ImGui::End();
}

gearoenix::editor::ui::MenuProject::MenuProject(Manager& manager)
    : manager(manager)
{
}

gearoenix::editor::ui::MenuProject::~MenuProject() = default;

void gearoenix::editor::ui::MenuProject::update()
{
    if (ImGui::BeginMenu("Project")) {
        if (ImGui::MenuItem("New", "Ctrl+N")) {
            is_new_popup_open = true;
        }

        if (ImGui::MenuItem("Open", "Ctrl+O")) { }

        if (ImGui::BeginMenu("Open Recent")) {

            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Save", "Ctrl+S", false)) { }
        if (ImGui::MenuItem("Save As..", "Ctrl+Shift+S", false)) { }
        if (ImGui::MenuItem("Settings", "Ctrl+Alt+P")) {
            is_settings_open = true;
        }
        if (ImGui::MenuItem("Quit", "Ctrl+Shift+Q")) { }
        ImGui::EndMenu();
    }

    show_new_popup();
    show_settings();
}
