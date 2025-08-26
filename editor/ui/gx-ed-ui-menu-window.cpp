#include "gx-ed-ui-menu-window.hpp"
#include <gearoenix/platform/gx-plt-application.hpp>
#include <gearoenix/platform/gx-plt-runtime-configuration.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>

#include <ImGui/imgui.h>

gearoenix::editor::ui::MenuWindow::MenuWindow() = default;

void gearoenix::editor::ui::MenuWindow::update()
{
    if (ImGui::BeginMenu("Window")) {
        if (const auto is_fullscreen = platform::RuntimeConfiguration::get().get_fullscreen();
            ImGui::MenuItem(is_fullscreen ? "Unset Fullscreen" : "Set Fullscreen", "F11")) {
            platform::Application::get().set_window_fullscreen(!is_fullscreen);
        }
        if (ImGui::Checkbox("Show Project Inspector", &show_engine_inspector)) {
        }
        ImGui::EndMenu();
    }

    if (show_engine_inspector) {
        if (ImGui::Begin("Gearoenix Render Engine", &show_engine_inspector)) {
            render::engine::Engine::get().show_debug_gui();
        }
        ImGui::End();
    }
}

void gearoenix::editor::ui::MenuWindow::renew()
{
    GX_TODO;
}
