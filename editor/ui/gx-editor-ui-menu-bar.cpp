#include "gx-editor-ui-menu-bar.hpp"
#include "../control/gx-editor-ctrl-manager.hpp"

#include <gearoenix/platform/gx-plt-log.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>
#include <gearoenix/platform/gx-plt-application.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-manager.hpp>

#include <imgui.h>
#include <imgui_stdlib.h>

constexpr static const char *const key_gltf_file_chooser = "key_gltf_file_chooser";
constexpr static const char *const  filter_gltf_file = ".gltf,.glb";

void gearoenix::editor::ui::MenuBar::show_project() noexcept {
    auto& project = control_manager.get_project();
    if (ImGui::BeginMenu("Project"))
    {
        if (ImGui::MenuItem("New", "Ctrl+N"))
            show_project_new_popup = true;

        if (ImGui::MenuItem("Open", "Ctrl+O")) {}

        const auto& recent_files = project.get_recent_file_addresses();
        if (ImGui::BeginMenu("Open Recent", !recent_files.empty()))
        {
            for(const auto& f: recent_files) {
                ImGui::MenuItem(f.c_str());
            }
            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Save", "Ctrl+S", false, project.is_project_opened())) {}
        if (ImGui::MenuItem("Save As..", "Ctrl+Shift+S", false, project.is_project_opened())) {}
        if (ImGui::MenuItem("Settings", "Ctrl+Alt+P", false, project.is_project_opened())) {}
        if (ImGui::MenuItem("Quit", "Ctrl+Shift+Q")) {}
        ImGui::EndMenu();
    }



    if(show_project_new_popup) {
        ImGui::OpenPopup("New Project");
        ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        if (ImGui::BeginPopupModal("New Project", &show_project_new_popup, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::InputText("New project name", &project.get_project_name());
            if(!project.get_project_name().empty()) {
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

void gearoenix::editor::ui::MenuBar::show_scene() noexcept {
    if (ImGui::BeginMenu("Scene"))
    {
        auto& project = control_manager.get_project();
        if (ImGui::MenuItem("New", "Alt+S,Alt+N", false, project.is_project_opened())) {}
        if (ImGui::MenuItem("Import", "Alt+S,Alt+I", false, project.is_project_opened())) {
//            show_scene_import_popup = true;
            //ImGuiFileDialog::Instance()->OpenDialog(key_gltf_file_chooser, "Import GLTF file", filter_gltf_file, ".");
        }
        if (ImGui::MenuItem("Delete", "Alt+S,Alt+Del", false, false)) {}
        if (ImGui::MenuItem("Settings", "Alt+S,Alt+S", false, false)) {}
        ImGui::EndMenu();
    }
//    if(show_scene_import_popup) {

        // display
        //if (ImGuiFileDialog::Instance()->Display(key_gltf_file_chooser))
        //{
        //    // action if OK
        //    if (ImGuiFileDialog::Instance()->IsOk())
        //    {
        //        std::string file_path_name = ImGuiFileDialog::Instance()->GetFilePathName();
        //        GX_TODO;
        //        // platform_application.get_base().get_render_engine()->get_scene_manager()->load_gltf(
        //        //         platform::stream::AbsolutePath(file_path_name));
        //    }

        //    // close
        //    ImGuiFileDialog::Instance()->Close();
        //}
//    }
}

gearoenix::editor::ui::MenuBar::MenuBar(
        platform::Application& platform_application,
        editor::control::Manager& control_manager) noexcept
    : platform_application(platform_application)
    , control_manager(control_manager)
{
}

void gearoenix::editor::ui::MenuBar::update() noexcept {
    if (ImGui::BeginMainMenuBar())
    {
        show_project();
        show_scene();
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
            const bool is_fullscreen = platform_application.get_base().get_configuration().get_fullscreen();
            if (ImGui::MenuItem(is_fullscreen? "Unset Fullscreen": "Set Fullscreen", "F11")) {
                platform_application.set_window_fullscreen(!is_fullscreen);
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
