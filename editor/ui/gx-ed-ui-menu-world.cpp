#include "gx-ed-ui-menu-world.hpp"

#include "editor/gx-editor-main.hpp"
#include "gx-ed-ui-menu-scene.hpp"
#include "gx-ed-ui-window-overlay-progress-bar.hpp"

#include <gearoenix/core/ecs/gx-cr-ecs-world.hpp>
#include <gearoenix/platform/gx-plt-file-chooser.hpp>
#include <gearoenix/platform/gx-plt-runtime-configuration.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-local.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-memory.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-stream.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/gx-rnd-runtime-configuration.hpp>
#include <gearoenix/render/imgui/gx-rnd-imgui-popup.hpp>
#include <gearoenix/render/imgui/gx-rnd-imgui-styles.hpp>

#include <ImGui/imgui.h>

namespace {
constexpr char file_chooser_filter[] = ".gx-world";

void save_world()
{
    auto progress_bar = gearoenix::editor::ui::WindowOverlayProgressBarManager::get().add("Saving World...");
    gearoenix::core::job::send_job_to_pool([progress_bar = std::move(progress_bar)]() mutable noexcept -> void {
        std::shared_ptr<gearoenix::platform::stream::Stream> stream = std::make_shared<gearoenix::platform::stream::Memory>();
        gearoenix::core::ecs::World::get().write(
            std::shared_ptr(stream),
            gearoenix::core::job::EndCaller([progress_bar = std::move(progress_bar), stream] {
                gearoenix::platform::file_chooser_save("world", "Save the World", file_chooser_filter, stream->get_file_content(), [] { });
            }));
    });
}

void open_world()
{
    gearoenix::platform::file_chooser_open(
        [](gearoenix::platform::stream::Path&& path, std::shared_ptr<gearoenix::platform::stream::Stream>&& stream) {
            gearoenix::core::ecs::World::get().clear();
            auto progress_bar = gearoenix::editor::ui::WindowOverlayProgressBarManager::get().add("Opening World...");
            gearoenix::core::ecs::World::read(std::move(stream), gearoenix::core::job::EndCaller<std::vector<gearoenix::core::ecs::EntityPtr>>([progress_bar = std::move(progress_bar)](auto&& entities) {
                for (auto& e : entities) {
                    gearoenix::editor::ui::MenuScene::get().add_active_scene(std::move(e));
                }
            }));
        },
        [] {}, "Open the World", file_chooser_filter);
}
}

void gearoenix::editor::ui::MenuWorld::show_new_popup()
{
    static constexpr char name[] = "Start a new project?";
    static constexpr char body[] = "Are you sure you want to start a new project?\nYou will loose your current unsaved work!";
    static const std::function<void()> fun = [] { core::Singleton<EditorApplication>::get().renew(); };

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
    static const std::function<void()> fun = [] { core::Singleton<EditorApplication>::get().quit(); };

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
            open_world();
        }

        if (!recent_save_files.empty() && ImGui::BeginMenu("Open Recent")) {
            ImGui::EndMenu();
        }

        if (has_active_save_file && ImGui::MenuItem("Save", "Ctrl+S", false)) { }

        if (ImGui::MenuItem("Save As..", "Ctrl+Shift+S", false)) {
            save_world();
        }

        if (ImGui::MenuItem("Settings", "Ctrl+Alt+P")) {
            is_settings_open = true;
        }

        if (ImGui::MenuItem("Quit", "Ctrl+Shift+Q")) {
            is_quit_popup_open = true;
        }

        ImGui::EndMenu();
    }

    show_new_popup();
    show_settings();
    show_quit_popup();
}

void gearoenix::editor::ui::MenuWorld::renew() { }
