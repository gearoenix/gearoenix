#include "gx-rnd-imgui-popup.hpp"
#include "gx-rnd-imgui-styles.hpp"

#include <ImGui/imgui.h>

void gearoenix::render::imgui::show_sure_popup(const char* const name, bool& is_open, const char* const body_text, const std::function<void()>& sure_fun)
{
    if (!is_open) {
        return;
    }

    ImGui::OpenPopup(name);
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    if (!ImGui::BeginPopupModal(name, &is_open, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) {
        return;
    }
    warning_text([&] {
        ImGui::Text(body_text);
    });
    const float window_width = ImGui::GetWindowSize().x;

    constexpr auto button_width_ratio = 0.35f;
    constexpr auto spacing_width_ratio = (1.0f - button_width_ratio * 2.0f) / 3.0f;
    constexpr auto sure_button_width = button_width_ratio + spacing_width_ratio * 2.0f;

    ImGui::SetCursorPosX(spacing_width_ratio * window_width);
    const auto button_width = window_width * button_width_ratio;
    if (safe_button([&] { return ImGui::Button("Cancel", ImVec2(button_width, 0.0f)); })) {
        is_open = false;
    }
    ImGui::SameLine();
    ImGui::SetCursorPosX(sure_button_width * window_width);
    if (dangerous_button([&] { return ImGui::Button("Sure!", ImVec2(button_width, 0.0f)); })) {
        is_open = false;
        sure_fun();
    }
    ImGui::EndPopup();
}