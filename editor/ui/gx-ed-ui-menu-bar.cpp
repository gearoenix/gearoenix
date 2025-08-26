#include "gx-ed-ui-menu-bar.hpp"
#include "gx-ed-ui-manager.hpp"
#include "gx-ed-ui-menu-entity.hpp"
#include "gx-ed-ui-menu-scene.hpp"
#include "gx-ed-ui-menu-window.hpp"
#include "gx-ed-ui-menu-world.hpp"
#include "gx-ed-ui-window-overlay-progress-bar.hpp"

#include <ImGui/imgui.h>

gearoenix::editor::ui::MenuBar::MenuBar()
    : entity_menu(new MenuEntity())
    , project_menu(new MenuWorld())
    , scene_menu(new MenuScene())
    , window_menu(new MenuWindow())
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

void gearoenix::editor::ui::MenuBar::renew()
{
    entity_menu->renew();
    project_menu->renew();
    scene_menu->renew();
    window_menu->renew();
}
