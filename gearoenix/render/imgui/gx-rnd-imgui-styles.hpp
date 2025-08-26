#pragma once
#include <functional>
#include <type_traits>
#include <utility>

#include <ImGui/imgui.h>

namespace gearoenix::render::imgui {

template <typename F>
[[nodiscard]] std::invoke_result_t<F> warning_text(F&& fun)
{
    [[maybe_unused]] const struct StylesPopper final {
        ~StylesPopper() { ImGui::PopStyleColor(); }
    } _style_popper {};

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.00f, 0.84f, 0.20f, 1.00f));

    return fun();
}

template <typename F>
[[nodiscard]] std::invoke_result_t<F> dangerous_button(F&& fun)
{
    [[maybe_unused]] const struct StylesPopper final {
        ~StylesPopper() { ImGui::PopStyleColor(3); }
    } _style_popper {};

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.45f, 0.15f, 0.15f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.90f, 0.30f, 0.30f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.90f, 0.20f, 0.20f, 1.00f));

    return fun();
}

template <typename F>
[[nodiscard]] std::invoke_result_t<F> safe_button(F&& fun)
{
    [[maybe_unused]] const struct StylesPopper final {
        ~StylesPopper() { ImGui::PopStyleColor(3); }
    } _style_popper {};

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.45f, 0.15f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.30f, 0.80f, 0.30f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.20f, 0.80f, 0.20f, 1.00f));

    return fun();
}

}