#include "gx-ed-ui-theme.hpp"

#include <ImGui/imgui.h>

void gearoenix::editor::ui::apply_modern_dark_theme()
{
    auto& style = ImGui::GetStyle();
    auto& c = style.Colors;

    // Rounded, breathable layout (CLion / Blender 4 feel).
    style.WindowRounding = 6.0f;
    style.ChildRounding = 5.0f;
    style.FrameRounding = 4.0f;
    style.PopupRounding = 6.0f;
    style.ScrollbarRounding = 6.0f;
    style.GrabRounding = 4.0f;
    style.TabRounding = 5.0f;
    style.WindowBorderSize = 1.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;
    style.TabBarBorderSize = 1.0f;
    style.WindowPadding = ImVec2(10.0f, 10.0f);
    style.FramePadding = ImVec2(8.0f, 5.0f);
    style.ItemSpacing = ImVec2(8.0f, 6.0f);
    style.ItemInnerSpacing = ImVec2(6.0f, 5.0f);
    style.IndentSpacing = 18.0f;
    style.ScrollbarSize = 14.0f;
    style.GrabMinSize = 12.0f;
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);

    // Default UI text size. Manager::update() may overwrite this every frame when the
    // user changes their preferred UI scale, so everything (widgets, icons, row heights)
    // rescales coherently via ImGui 1.92's dynamic font atlas.
    style.FontSizeBase = 15.0f;

    constexpr ImVec4 bg_deep { 0.082f, 0.082f, 0.086f, 1.000f };
    constexpr ImVec4 bg { 0.115f, 0.115f, 0.120f, 1.000f };
    constexpr ImVec4 panel { 0.160f, 0.160f, 0.165f, 1.000f };
    constexpr ImVec4 panel_h { 0.205f, 0.205f, 0.212f, 1.000f };
    constexpr ImVec4 panel_a { 0.255f, 0.255f, 0.262f, 1.000f };
    constexpr ImVec4 border { 0.240f, 0.240f, 0.250f, 1.000f };
    constexpr ImVec4 text_c { 0.910f, 0.915f, 0.925f, 1.000f };
    constexpr ImVec4 text_d { 0.500f, 0.510f, 0.535f, 1.000f };
    // Purplish accent — same family as the viewport's toggled-button fill
    // (`IM_COL32(90, 90, 200, 200)` in `gx-ed-vp-{translate, rotate, scale}-button.cpp`)
    // so hovered / active widgets, selection highlights, and toggled viewport buttons
    // all read as members of the same theme.
    constexpr ImVec4 accent { 0.450f, 0.340f, 0.820f, 1.000f };
    constexpr ImVec4 accent_h { 0.565f, 0.450f, 0.940f, 1.000f };
    constexpr ImVec4 accent_dim { 0.450f, 0.340f, 0.820f, 0.400f };
    constexpr ImVec4 clear { 0.000f, 0.000f, 0.000f, 0.000f };

    c[ImGuiCol_Text] = text_c;
    c[ImGuiCol_TextDisabled] = text_d;
    c[ImGuiCol_WindowBg] = bg;
    c[ImGuiCol_ChildBg] = bg;
    c[ImGuiCol_PopupBg] = panel;
    c[ImGuiCol_Border] = border;
    c[ImGuiCol_BorderShadow] = clear;
    c[ImGuiCol_FrameBg] = panel;
    c[ImGuiCol_FrameBgHovered] = panel_h;
    c[ImGuiCol_FrameBgActive] = panel_a;
    c[ImGuiCol_TitleBg] = bg_deep;
    c[ImGuiCol_TitleBgActive] = panel;
    c[ImGuiCol_TitleBgCollapsed] = bg_deep;
    c[ImGuiCol_MenuBarBg] = panel;
    c[ImGuiCol_ScrollbarBg] = bg;
    c[ImGuiCol_ScrollbarGrab] = ImVec4(0.330f, 0.330f, 0.343f, 1.000f);
    c[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.450f, 0.450f, 0.462f, 1.000f);
    c[ImGuiCol_ScrollbarGrabActive] = accent;
    c[ImGuiCol_CheckMark] = accent_h;
    c[ImGuiCol_SliderGrab] = accent;
    c[ImGuiCol_SliderGrabActive] = accent_h;
    c[ImGuiCol_Button] = panel_a;
    c[ImGuiCol_ButtonHovered] = accent_dim;
    c[ImGuiCol_ButtonActive] = accent;
    c[ImGuiCol_Header] = panel_a;
    c[ImGuiCol_HeaderHovered] = accent_dim;
    c[ImGuiCol_HeaderActive] = accent;
    c[ImGuiCol_Separator] = border;
    c[ImGuiCol_SeparatorHovered] = accent;
    c[ImGuiCol_SeparatorActive] = accent_h;
    c[ImGuiCol_ResizeGrip] = clear;
    c[ImGuiCol_ResizeGripHovered] = accent_dim;
    c[ImGuiCol_ResizeGripActive] = accent;
    c[ImGuiCol_InputTextCursor] = text_c;
    c[ImGuiCol_Tab] = panel;
    c[ImGuiCol_TabHovered] = accent_dim;
    c[ImGuiCol_TabSelected] = panel_a;
    c[ImGuiCol_TabSelectedOverline] = accent_h;
    c[ImGuiCol_TabDimmed] = panel;
    c[ImGuiCol_TabDimmedSelected] = panel_h;
    c[ImGuiCol_TabDimmedSelectedOverline] = border;
    c[ImGuiCol_PlotLines] = accent_h;
    c[ImGuiCol_PlotLinesHovered] = accent;
    c[ImGuiCol_PlotHistogram] = accent_h;
    c[ImGuiCol_PlotHistogramHovered] = accent;
    c[ImGuiCol_TableHeaderBg] = panel;
    c[ImGuiCol_TableBorderStrong] = border;
    c[ImGuiCol_TableBorderLight] = ImVec4(0.185f, 0.185f, 0.192f, 1.000f);
    c[ImGuiCol_TableRowBg] = clear;
    c[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.028f);
    c[ImGuiCol_TextLink] = accent_h;
    c[ImGuiCol_TextSelectedBg] = accent_dim;
    c[ImGuiCol_TreeLines] = border;
    c[ImGuiCol_DragDropTarget] = accent_h;
    c[ImGuiCol_NavCursor] = accent;
    c[ImGuiCol_NavWindowingHighlight] = accent;
    c[ImGuiCol_NavWindowingDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.5f);
    c[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.55f);
}
