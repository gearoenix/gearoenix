#include "gx-ed-ui-menu-bar.hpp"
#include "gx-ed-ui-manager.hpp"
#include "gx-ed-ui-menu-entity.hpp"
#include "gx-ed-ui-menu-scene.hpp"
#include "gx-ed-ui-menu-window.hpp"
#include "gx-ed-ui-menu-world.hpp"
#include "gx-ed-ui-window-overlay-progress-bar.hpp"
#include <imgui/imgui.h>

gearoenix::editor::ui::MenuBar::MenuBar(Manager& manager)
    : manager(manager)
    , entity_menu(new MenuEntity(manager))
    , project_menu(new MenuWorld(manager))
    , scene_menu(new MenuScene(manager))
    , window_menu(new MenuWindow(manager))
{
}

gearoenix::editor::ui::MenuBar::~MenuBar() = default;

void gearoenix::editor::ui::MenuBar::update()
{
    if (ImGui::BeginMainMenuBar()) {
        project_menu->update();
        scene_menu->update();
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
