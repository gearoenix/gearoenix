#include "gx-editor-ui-menu-window.hpp"
#include "gx-editor-ui-manager.hpp"
#include <gearoenix/platform/gx-plt-application.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <imgui/imgui.h>

gearoenix::editor::ui::MenuWindow::MenuWindow(Manager& manager)
    : manager(manager)
{
}

void gearoenix::editor::ui::MenuWindow::update()
{
    if (ImGui::BeginMenu("Window")) {
        const auto is_fullscreen = manager.get_platform_application().get_base().get_configuration().get_fullscreen();
        if (ImGui::MenuItem(is_fullscreen ? "Unset Fullscreen" : "Set Fullscreen", "F11")) {
            manager.get_platform_application().set_window_fullscreen(!is_fullscreen);
        }
        if (ImGui::Checkbox("Show Project Inspector", &show_engine_inspector)) {
        }
        ImGui::EndMenu();
    }

    if (show_engine_inspector) {
        if (ImGui::Begin("Gearoenix Render Engine", &show_engine_inspector)) {
            manager.get_platform_application().get_base().get_render_engine()->show_debug_gui();
        }
        ImGui::End();
    }
}