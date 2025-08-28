#pragma once
#include <imgui/imgui.h>

namespace gearoenix::render::imgui {
struct WrongInputTextStyle;

[[nodiscard]] WrongInputTextStyle set_wrong_input_text_style(bool is_valid);

struct WrongInputTextStyle final {
    friend WrongInputTextStyle set_wrong_input_text_style(bool);

    const bool is_valid;

private:
    explicit WrongInputTextStyle(const bool is_invalid)
        : is_valid(is_invalid)
    {
    }

public:
    ~WrongInputTextStyle()
    {
        if (is_valid) {
            return;
        }
        ImGui::PopStyleColor(2);
    }
};

[[nodiscard]] inline WrongInputTextStyle set_wrong_input_text_style(const bool is_valid)
{
    if (is_valid) {
        return WrongInputTextStyle { true };
    }
    ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(255, 0, 0, 128));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(255, 0, 0, 150));
    return WrongInputTextStyle { false };
}
}