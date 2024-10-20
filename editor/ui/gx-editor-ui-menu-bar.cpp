#include "gx-editor-ui-menu-bar.hpp"
#include "gx-editor-ui-manager.hpp"
#include "gx-editor-ui-menu-entity.hpp"
#include "gx-editor-ui-menu-project.hpp"
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

void gearoenix::editor::ui::MenuBar::show_scene()
{
    if (ImGui::BeginMenu("Scene")) {
        if (ImGui::MenuItem("New", "Alt+S,Alt+N", false)) { }
        if (ImGui::MenuItem("Import", "Alt+S,Alt+I", false)) {
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
                *manager.get_platform_application().get_base().get_render_engine(),
                platform::stream::Path::create_absolute(std::move(file_path_name)),
                core::job::EndCaller<std::vector<std::shared_ptr<render::scene::Builder>>>([this, progress_bar_id](auto&& /*scene_builders*/) {
                    manager.get_window_overlay_progree_bar_manager()->remove(progress_bar_id);
                }),
                core::job::EndCaller([] {}));
        }
        ImGuiFileDialog::Instance()->Close();
    }
}

gearoenix::editor::ui::MenuBar::MenuBar(Manager& manager)
    : manager(manager)
    , entity_menu(new MenuEntity(manager))
    , project_menu(new MenuProject(manager))
    , window_menu(new MenuWindow(manager))
{
}

gearoenix::editor::ui::MenuBar::~MenuBar() = default;

void gearoenix::editor::ui::MenuBar::update()
{
    if (ImGui::BeginMainMenuBar()) {
        project_menu->update();
        show_scene();
        entity_menu->update();
        if (ImGui::BeginMenu("Audio")) {
            if (ImGui::MenuItem("Settings", "Ctrl+Alt+A", false, false)) { }
            ImGui::EndMenu();
        }
        window_menu->update();
        if (ImGui::BeginMenu("Help")) {
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}
