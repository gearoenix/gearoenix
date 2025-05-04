#include "gx-ed-ui-menu-world.hpp"
#include "gx-ed-ui-manager.hpp"
#include "gx-ed-ui-window-overlay-progress-bar.hpp"
#include <ImGuiFileDialog/ImGuiFileDialog.h>
#include <gearoenix/core/ecs/gx-cr-ecs-world.hpp>
#include <gearoenix/platform/gx-plt-runtime-configuration.hpp>
#include <gearoenix/platform/sdl2/gx-plt-sdl2-application.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-local.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-stream.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/gx-rnd-runtime-configuration.hpp>
#include <imgui/imgui.h>

namespace {
constexpr auto save_file_chooser = "gearoenix::editor::ui::MenuProject::save";
constexpr auto save_file_chooser_filter = ".gx-world";
}

void gearoenix::editor::ui::MenuWorld::show_new_popup()
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

void gearoenix::editor::ui::MenuWorld::show_settings()
{
    if (!is_settings_open) {
        return;
    }

    if (!ImGui::Begin("Project Settings", &is_settings_open)) {
        ImGui::End();
        return;
    }

    if (ImGui::TreeNode("Platform Settings")) {
        core::ecs::Singleton::get<platform::RuntimeConfiguration>().show_debug_gui();
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Render Settings")) {
        core::ecs::Singleton::get<render::RuntimeConfiguration>().show_debug_gui();
        ImGui::TreePop();
    }

    ImGui::End();
}

gearoenix::editor::ui::MenuWorld::MenuWorld(Manager& manager)
    : manager(manager)
{
}

gearoenix::editor::ui::MenuWorld::~MenuWorld() = default;

void gearoenix::editor::ui::MenuWorld::update()
{
    if (ImGui::BeginMenu("World")) {
        if (ImGui::MenuItem("New", "Ctrl+N")) {
            is_new_popup_open = true;
        }

        if (ImGui::MenuItem("Open", "Ctrl+O")) {
            is_file_chooser_open = true;
            file_chooser_title = "Opening a Gearoenix World File";
            ImGuiFileDialog::Instance()->OpenDialog(save_file_chooser, file_chooser_title, save_file_chooser_filter, { .flags = ImGuiFileDialogFlags_ReadOnlyFileNameField });
        }

        if (ImGui::BeginMenu("Open Recent")) {

            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Save", "Ctrl+S", false)) { }
        if (ImGui::MenuItem("Save As..", "Ctrl+Shift+S", false)) {
            is_file_chooser_open = false;
            file_chooser_title = "Saving a Gearoenix World File";
            ImGuiFileDialog::Instance()->OpenDialog(save_file_chooser, file_chooser_title, save_file_chooser_filter);
        }
        if (ImGui::MenuItem("Settings", "Ctrl+Alt+P")) {
            is_settings_open = true;
        }
        if (ImGui::MenuItem("Quit", "Ctrl+Shift+Q")) { }
        ImGui::EndMenu();
    }

    if (ImGuiFileDialog::Instance()->Display(save_file_chooser)) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            const auto progress_bar_id = manager.get_window_overlay_progree_bar_manager()->add(std::string("Running Process [") + file_chooser_title + "]");
            const std::shared_ptr<platform::stream::Stream> stream(platform::stream::Local::open(
                manager.get_platform_application(), ImGuiFileDialog::Instance()->GetFilePathName(), !is_file_chooser_open));
            if (is_file_chooser_open) {
                core::ecs::World::get()->read(
                    stream, core::job::EndCaller([this, progress_bar_id] {
                        manager.get_window_overlay_progree_bar_manager()->remove(progress_bar_id);
                    }));

            } else {
                core::ecs::World::get()->write(
                    stream, core::job::EndCaller([this, progress_bar_id] {
                        manager.get_window_overlay_progree_bar_manager()->remove(progress_bar_id);
                    }));
            }
        }
        ImGuiFileDialog::Instance()->Close();
    }

    show_new_popup();
    show_settings();
}
