#include "gx-editor-ui-utility.hpp"
#include <imgui/imgui.h>

gearoenix::editor::ui::Utility::WrongInputTextStyle::WrongInputTextStyle(const bool is_invalid)
    : is_invalid(is_invalid)
{
}

gearoenix::editor::ui::Utility::WrongInputTextStyle::~WrongInputTextStyle()
{
    if (is_invalid) {
        return;
    }
    ImGui::PopStyleColor(2);
}

gearoenix::editor::ui::Utility::WrongInputTextStyle gearoenix::editor::ui::Utility::set_wrong_input_text(
    const bool is_valid)
{
    if (is_valid) {
        return WrongInputTextStyle { true };
    }
    ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(255, 0, 0, 128));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(255, 0, 0, 150));
    return WrongInputTextStyle { false };
}
