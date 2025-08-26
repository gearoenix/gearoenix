#include "gx-ed-vp-viewport.hpp"
#include "gx-ed-vp-camera.hpp"
#include "gx-ed-vp-gizmo-buttons.hpp"
#include "gx-ed-vp-projection-button.hpp"

#include <gearoenix/core/gx-cr-string.hpp>
#include <gearoenix/math/gx-math-imgui.hpp>

#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>

#include <string>

gearoenix::editor::viewport::Viewport::Viewport()
    : Singleton(this)
    , invisible_window_id("##window-" + core::String::ptr_name(this))
    , projection_button(new ProjectionButton())
    , gizmo_buttons(new GizmoButtons(*projection_button))
    , camera(new Camera())
{
}

gearoenix::editor::viewport::Viewport::~Viewport() = default;

void gearoenix::editor::viewport::Viewport::update()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Always);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    [[maybe_unused]] const auto is_window_open = ImGui::Begin(
        invisible_window_id.c_str(), nullptr,
        ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | // --------------------
            ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoNavFocus | // ----
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
    GX_ASSERT_D(is_window_open);

    camera->update();
    projection_button->update();
    gizmo_buttons->update();

    ImGui::End();
    ImGui::PopStyleVar();
}

void gearoenix::editor::viewport::Viewport::renew()
{
}
