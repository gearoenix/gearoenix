#pragma once

#include <ImGui/imgui.h>

namespace gearoenix::editor::ui {
/// Show a plain-text tooltip for the previous item if it is hovered. Delegates to
/// `ImGui::SetItemTooltip` (available since 1.89.7) which performs the hover check
/// internally with `ImGuiHoveredFlags_ForTooltip`, using the style-driven delay. The
/// `%s` format protects against any `%`-bearing text without us having to sanitise it.
inline void tooltip(const char* const text)
{
    if (text != nullptr && text[0] != '\0') {
        ImGui::SetItemTooltip("%s", text);
    }
}
}
