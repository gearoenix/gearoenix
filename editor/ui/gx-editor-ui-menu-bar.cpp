#include "gx-editor-ui-menu-bar.hpp"
#include "../control/gx-editor-ctrl-manager.hpp"
#include <gearoenix/platform/gx-plt-application.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-manager.hpp>
#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

constexpr static auto key_gltf_file_chooser = "key_gltf_file_chooser";
constexpr static auto filter_gltf_file = ".gltf,.glb";

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
        ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
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
            //            show_scene_import_popup = true;
            // ImGuiFileDialog::Instance()->OpenDialog(key_gltf_file_chooser, "Import GLTF file", filter_gltf_file, ".");
        }
        if (ImGui::MenuItem("Delete", "Alt+S,Alt+Del", false, false)) { }
        if (ImGui::MenuItem("Settings", "Alt+S,Alt+S", false, false)) { }
        ImGui::EndMenu();
    }
    //    if(show_scene_import_popup) {

    // display
    // if (ImGuiFileDialog::Instance()->Display(key_gltf_file_chooser))
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
    control::Manager& control_manager)
    : platform_application(platform_application)
    , control_manager(control_manager)
{
}

void gearoenix::editor::ui::MenuBar::update()
{
    if (ImGui::BeginMainMenuBar()) {
        show_project();
        show_scene();
        if (ImGui::BeginMenu("Object")) {
            if (ImGui::MenuItem("New", "Alt+O,Alt+N", false, false)) { }
            if (ImGui::MenuItem("Delete", "Alt+O,Alt+Del", false, false)) { }
            if (ImGui::MenuItem("Settings", "Ctrl+Alt+O", false, false)) { }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Audio")) {
            if (ImGui::MenuItem("Settings", "Ctrl+Alt+A", false, false)) { }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Window")) {
            const bool is_fullscreen = platform_application.get_base().get_configuration().get_fullscreen();
            if (ImGui::MenuItem(is_fullscreen ? "Unset Fullscreen" : "Set Fullscreen", "F11")) {
                platform_application.set_window_fullscreen(!is_fullscreen);
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help")) {
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    {
        static float h_splitter = 150.0f;
        static float v_splitter = 200.0f;
        static bool p_open = true;

        ImGui::Begin("Example: Horizontal Splitter", &p_open, ImGuiWindowFlags_MenuBar);

        ImGui::Text("Drag the splitter to resize the sections.");

        ImVec2 window_size = ImGui::GetContentRegionAvail();
        float x_offset = window_size.x;

        ImGui::Separator();
        ImGui::BeginChild("TopRegion", ImVec2(0, h_splitter), true);
        ImGui::Text("Top Region");
        ImGui::EndChild();

        ImGui::Separator();
        ImGui::BeginChild("Splitter", ImVec2(0, 4), false);
        if (ImGui::Button("##splitter", ImVec2(-1, 4))) {
            ImGui::SetItemDefaultFocus();
            if (ImGui::IsItemActive()) {
                h_splitter += ImGui::GetIO().MouseDelta.y;
            }
        }
        ImGui::EndChild();
        ImGui::Separator();

        ImGui::BeginChild("BottomRegion", ImVec2(0, 0), true);
        ImGui::Text("Bottom Region");
        ImGui::EndChild();

        ImGui::End();
    }
}
