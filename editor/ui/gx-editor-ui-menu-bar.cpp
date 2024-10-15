#include "gx-editor-ui-menu-bar.hpp"
#include "../control/gx-editor-ctrl-manager.hpp"
#include "gx-editor-ui-manager.hpp"
#include "gx-editor-ui-menu-entity.hpp"
#include "gx-editor-ui-menu-window.hpp"
#include "gx-editor-ui-window-overlay-progress-bar.hpp"
#include <ImGuiFileDialog/ImGuiFileDialog.h>
#include <gearoenix/platform/gx-plt-application.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>
#include <gearoenix/render/gx-rnd-gltf-loader.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-manager.hpp>
#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

constexpr static auto key_gltf_file_chooser = "key_gltf_file_chooser";
constexpr static auto filter_gltf_file = ".glb,.gltf";

void gearoenix::editor::ui::MenuBar::show_project()
{
    auto& project = control_manager.get_project();
    if (ImGui::BeginMenu("Project")) {
        if (ImGui::MenuItem("New", "Ctrl+N"))
            show_project_new_popup = true;

        if (ImGui::MenuItem("Open", "Ctrl+O")) { }

        const auto& recent_files = project.get_recent_file_addresses();
        if (ImGui::BeginMenu("Open Recent", !recent_files.empty())) {
            for (const auto& f : recent_files) {
                ImGui::MenuItem(f.c_str());
            }
            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Save", "Ctrl+S", false, project.is_project_opened())) { }
        if (ImGui::MenuItem("Save As..", "Ctrl+Shift+S", false, project.is_project_opened())) { }
        if (ImGui::MenuItem("Settings", "Ctrl+Alt+P", false, project.is_project_opened())) { }
        if (ImGui::MenuItem("Quit", "Ctrl+Shift+Q")) { }
        ImGui::EndMenu();
    }

    if (show_project_new_popup) {
        ImGui::OpenPopup("New Project");
        const ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        if (ImGui::BeginPopupModal("New Project", &show_project_new_popup, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::InputText("New project name", &project.get_project_name());
            if (!project.get_project_name().empty()) {
                if (ImGui::Button("Create")) {
                    // TODO check for existence of a current unsaved project
                    project.create_project();
                    show_project_new_popup = false;
                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::EndPopup();
        }
    }
}

void gearoenix::editor::ui::MenuBar::show_scene()
{
    if (ImGui::BeginMenu("Scene")) {
        auto& project = control_manager.get_project();
        if (ImGui::MenuItem("New", "Alt+S,Alt+N", false, project.is_project_opened())) { }
        if (ImGui::MenuItem("Import", "Alt+S,Alt+I", false, project.is_project_opened())) {
            ImGuiFileDialog::Instance()->OpenDialog(key_gltf_file_chooser, "Import GLTF file", filter_gltf_file);
        }
        if (ImGui::MenuItem("Delete", "Alt+S,Alt+Del", false, false)) { }
        if (ImGui::MenuItem("Settings", "Alt+S,Alt+S", false, false)) { }
        ImGui::EndMenu();
    }

    if (ImGuiFileDialog::Instance()->Display(key_gltf_file_chooser)) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            const auto progress_bar_id = manager.get_window_overlay_progree_bar_manager()->add("Loading Scenes from GLTF File...");
            std::string file_path_name = ImGuiFileDialog::Instance()->GetFilePathName();
            render::gltf::load(
                *platform_application.get_base().get_render_engine(),
                platform::stream::Path::create_absolute(std::move(file_path_name)),
                core::job::EndCaller<std::vector<std::shared_ptr<render::scene::Builder>>>([this, progress_bar_id](auto&& /*scene_builders*/) {
                    manager.get_window_overlay_progree_bar_manager()->remove(progress_bar_id);
                }),
                core::job::EndCaller([] {}));
        }
        ImGuiFileDialog::Instance()->Close();
    }
}

gearoenix::editor::ui::MenuBar::MenuBar(
    platform::Application& platform_application,
    control::Manager& control_manager,
    Manager& manager)
    : platform_application(platform_application)
    , control_manager(control_manager)
    , manager(manager)
    , entity_menu(new MenuEntity(manager))
    , window(new MenuWindow(manager))
{
}

gearoenix::editor::ui::MenuBar::~MenuBar() = default;

void gearoenix::editor::ui::MenuBar::update()
{
    if (ImGui::BeginMainMenuBar()) {
        show_project();
        show_scene();
        entity_menu->update();
        if (ImGui::BeginMenu("Audio")) {
            if (ImGui::MenuItem("Settings", "Ctrl+Alt+A", false, false)) { }
            ImGui::EndMenu();
        }
        window->update();
        if (ImGui::BeginMenu("Help")) {
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}
