#include "gx-ed-ui-menu-world.hpp"
#include "editor/gx-editor-main.hpp"
#include "gx-ed-ui-window-overlay-progress-bar.hpp"

#include <gearoenix/core/ecs/gx-cr-ecs-world.hpp>
#include <gearoenix/platform/gx-plt-runtime-configuration.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-local.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-stream.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/gx-rnd-runtime-configuration.hpp>
#include <gearoenix/render/imgui/gx-rnd-imgui-popup.hpp>
#include <gearoenix/render/imgui/gx-rnd-imgui-styles.hpp>

#include <ImGui/imgui.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

namespace {
constexpr char save_file_chooser[] = "gearoenix::editor::ui::MenuProject::save";
constexpr char save_file_chooser_title[] = "Saving a Gearoenix World File";

constexpr char open_file_chooser[] = "gearoenix::editor::ui::MenuProject::open";
constexpr char open_file_chooser_title[] = "Opening a Gearoenix World File";

constexpr char file_chooser_filter[] = ".gx-world";
}

void gearoenix::editor::ui::MenuWorld::show_new_popup()
{
    static constexpr char name[] = "Start a new project?";
    static constexpr char body[] = "Are you sure you want to start a new project?\nYou will loose your current unsaved work!";
    static const std::function<void()> fun = [] {
        core::Singleton<EditorApplication>::get().renew();
    };

    render::imgui::show_sure_popup(name, is_new_popup_open, body, fun);
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
        platform::RuntimeConfiguration::get().show_debug_gui();
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Render Settings")) {
        render::RuntimeConfiguration::get().show_debug_gui();
        ImGui::TreePop();
    }

    ImGui::End();
}

void gearoenix::editor::ui::MenuWorld::show_quit_popup()
{
    static constexpr char name[] = "Quit the project?";
    static constexpr char body[] = "Are you sure, you want to quit the editor?\nYou will loose your current unsaved work!";
    static const std::function<void()> fun = [] {
        core::Singleton<EditorApplication>::get().quit();
    };

    render::imgui::show_sure_popup(name, is_quit_popup_open, body, fun);
}

gearoenix::editor::ui::MenuWorld::MenuWorld() = default;

gearoenix::editor::ui::MenuWorld::~MenuWorld() = default;

void gearoenix::editor::ui::MenuWorld::update()
{
    if (ImGui::BeginMenu("World")) {
        if (ImGui::MenuItem("New", "Ctrl+N")) {
            is_new_popup_open = true;
        }

        if (ImGui::MenuItem("Open", "Ctrl+O")) {
            is_file_chooser_open = true;
            ImGuiFileDialog::Instance()->OpenDialog(open_file_chooser, open_file_chooser_title, file_chooser_filter, { .flags = ImGuiFileDialogFlags_ReadOnlyFileNameField });
        }

        if (!recent_save_files.empty() && ImGui::BeginMenu("Open Recent")) {
            ImGui::EndMenu();
        }

        if (has_active_save_file && ImGui::MenuItem("Save", "Ctrl+S", false)) { }

        if (ImGui::MenuItem("Save As..", "Ctrl+Shift+S", false)) {
            is_file_chooser_save = true;
            ImGuiFileDialog::Instance()->OpenDialog(save_file_chooser, save_file_chooser_title, file_chooser_filter);
        }

        if (ImGui::MenuItem("Settings", "Ctrl+Alt+P")) {
            is_settings_open = true;
        }

        if (ImGui::MenuItem("Quit", "Ctrl+Shift+Q")) {
            is_quit_popup_open = true;
        }

        ImGui::EndMenu();
    }

    if (is_file_chooser_save) {
        if (ImGuiFileDialog::Instance()->Display(save_file_chooser)) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                const auto progress_bar_id = WindowOverlayProgressBarManager::get().add(std::string("Running Process [") + save_file_chooser_title + "]");
                const std::shared_ptr<platform::stream::Stream> stream(platform::stream::Local::open(ImGuiFileDialog::Instance()->GetFilePathName(), true));
                GX_TODO;
                // core::ecs::World::get()->write(
                //     stream, core::job::EndCaller([this, progress_bar_id] {
                //         manager.get_window_overlay_progress_bar_manager()->remove(progress_bar_id);
                //     }));
                is_file_chooser_save = false;
            }
            ImGuiFileDialog::Instance()->Close();
        }
        is_file_chooser_save = ImGuiFileDialog::Instance()->IsOpened();
    }

    if (is_file_chooser_open) {
        if (ImGuiFileDialog::Instance()->Display(open_file_chooser)) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                const auto progress_bar_id = WindowOverlayProgressBarManager::get().add(std::string("Running Process [") + open_file_chooser_title + "]");
                const std::shared_ptr<platform::stream::Stream> stream(platform::stream::Local::open(ImGuiFileDialog::Instance()->GetFilePathName()));
                GX_TODO;
                // core::ecs::World::get().read(
                //     stream, core::job::EndCaller([this, progress_bar_id] {
                //         manager.get_window_overlay_progress_bar_manager()->remove(progress_bar_id);
                //     }));
                is_file_chooser_open = false;
            }
            ImGuiFileDialog::Instance()->Close();
        }
        is_file_chooser_open = ImGuiFileDialog::Instance()->IsOpened();
    }

    show_new_popup();
    show_settings();
    show_quit_popup();
}

void gearoenix::editor::ui::MenuWorld::renew()
{
}
