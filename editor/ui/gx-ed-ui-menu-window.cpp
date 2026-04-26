#include "gx-ed-ui-menu-window.hpp"
#include "gx-ed-ui-components.hpp"
#include "gx-ed-ui-entities.hpp"
#include "gx-ed-ui-font.hpp"
#include "gx-ed-ui-side-panel.hpp"
#include "gx-ed-ui-tooltip.hpp"

#include <gearoenix/platform/gx-plt-application.hpp>
#include <gearoenix/platform/gx-plt-runtime-configuration.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>

#include <ImGui/imgui.h>

gearoenix::editor::ui::MenuWindow::MenuWindow() = default;

void gearoenix::editor::ui::MenuWindow::update()
{
    if (ImGui::BeginMenu("Window")) {
        if (const auto is_fullscreen = platform::RuntimeConfiguration::get().get_fullscreen(); ImGui::MenuItem(is_fullscreen ? "Unset Fullscreen" : "Set Fullscreen", "F11")) {
            platform::Application::get().set_window_fullscreen(!is_fullscreen);
        }
        tooltip("Toggle borderless fullscreen");

        ImGui::MenuItem("Side Panel", nullptr, &SidePanel::get().get_shown());
        tooltip("Show/hide the whole right-side panel (entities + components)");
        ImGui::MenuItem("Entities", nullptr, &Entities::get().shown);
        tooltip("Show/hide the scene-hierarchy entities panel");
        ImGui::MenuItem("Components", nullptr, &Components::get().shown);
        tooltip("Show/hide the components panel");
        ImGui::MenuItem("Show Project Inspector", nullptr, &show_engine_inspector);
        tooltip("Open the low-level render engine inspector");

        ImGui::Separator();

        // Text / UI scale. Drives every font-relative metric in the editor.
        float size_px = font::get_size_px();
        ImGui::SetNextItemWidth(140.0f);
        if (ImGui::SliderFloat("UI Text Size",
                &size_px,
                font::min_size_px,
                font::max_size_px,
                "%.0f px")) {
            font::set_size_px(size_px);
        }
        tooltip("Adjust the base UI font size (rescales the whole editor)");
        ImGui::SameLine();
        if (ImGui::SmallButton("Reset##ui-font")) {
            font::set_size_px(font::default_size_px);
        }
        tooltip("Reset UI text size to default");
        ImGui::EndMenu();
    }

    if (show_engine_inspector) {
        if (ImGui::Begin("Gearoenix Render Engine", &show_engine_inspector)) {
            render::engine::Engine::get().show_debug_gui();
        }
        ImGui::End();
    }
}

void gearoenix::editor::ui::MenuWindow::renew() { GX_TODO; }