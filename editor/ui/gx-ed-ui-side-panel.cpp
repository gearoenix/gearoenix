#include "gx-ed-ui-side-panel.hpp"

#include "gx-ed-ui-components.hpp"
#include "gx-ed-ui-dockable-section.hpp"
#include "gx-ed-ui-entities.hpp"
#include "gx-ed-ui-icons.hpp"
#include "gx-ed-ui-tooltip.hpp"

#include <ImGui/imgui.h>

#include <algorithm>

namespace {
// ---------------------------------------------------------------------------
//  Layout constants.
//
//  Splitter/handle thickness is expressed as a ratio of the smaller screen
//  dimension — the same convention ImGuizmo uses — so the hit regions scale
//  naturally on very wide or very tall displays without ballooning on one axis.
//  The section-height ratio bounds keep the drag splitter from collapsing a
//  section to nothing or pushing it off-screen.
//
//  The horizontal splitter between the two sections reuses the same thickness
//  as the panel's vertical resize handle on the left edge, so both feel like
//  the same widget — a thin, unobtrusive grabber.
// ---------------------------------------------------------------------------
constexpr float handle_thickness_ratio = 0.0025f;
constexpr float min_handle_thickness_px = 3.0f;
constexpr float min_section_height_ratio = 0.12f;
constexpr float max_section_height_ratio = 0.88f;
constexpr float min_panel_width_px = 240.0f;

/// Shared child-window flags for the two layout sub-columns of the side panel.
constexpr auto borderless_child_flags
    = ImGuiWindowFlags_NoScrollbar
    | ImGuiWindowFlags_NoScrollWithMouse
    | ImGuiWindowFlags_NoBackground;

/// Window flags for the outer side panel — pinned to the right edge, no scroll,
/// not focusable (so it doesn't steal input focus from the viewport).
constexpr auto side_panel_window_flags
    = ImGuiWindowFlags_NoMove
    | ImGuiWindowFlags_NoResize
    | ImGuiWindowFlags_NoCollapse
    | ImGuiWindowFlags_NoTitleBar
    | ImGuiWindowFlags_NoScrollbar
    | ImGuiWindowFlags_NoScrollWithMouse
    | ImGuiWindowFlags_NoBringToFrontOnFocus
    | ImGuiWindowFlags_NoSavedSettings;

/// The smaller of the main viewport's two work-area dimensions, clamped to a safe
/// minimum so layout fractions never produce subpixel widgets on tiny viewports.
[[nodiscard]] float min_screen_dim()
{
    const auto* const vp = ImGui::GetMainViewport();
    return std::max(100.0f, std::min(vp->WorkSize.x, vp->WorkSize.y));
}

/// True when at least one of the panel's two sections is currently docked. When false,
/// the side panel is skipped entirely, so the viewport reclaims the horizontal space.
[[nodiscard]] bool wants_panel()
{
    return gearoenix::editor::ui::Entities::get().is_docked() || gearoenix::editor::ui::Components::get().is_docked();
}
}

gearoenix::editor::ui::SidePanel::SidePanel()
    : Singleton(this)
{
}

void gearoenix::editor::ui::SidePanel::draw_sections_splitter(
    const float current_first_section_h, const float full_h, const float thickness_px)
{
    // A button styled as a separator — transparent when idle, accent on hover/drag.
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_SeparatorHovered));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetStyleColorVec4(ImGuiCol_SeparatorActive));
    ImGui::Button("##splitter", ImVec2(-1.0f, thickness_px));

    if (ImGui::IsItemActive()) {
        const float dy = ImGui::GetIO().MouseDelta.y;
        first_section_height_ratio = std::clamp(
            (current_first_section_h + dy) / std::max(1.0f, full_h),
            min_section_height_ratio, max_section_height_ratio);
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
    }
    ImGui::PopStyleColor(3);
}

void gearoenix::editor::ui::SidePanel::draw_panel_resize_handle(
    const float panel_width, const float work_w)
{
    const float handle_w = std::max(min_handle_thickness_px, handle_thickness_ratio * min_screen_dim());

    if (ImGui::BeginChild("##panel-resize-col", ImVec2(handle_w, 0),
            ImGuiChildFlags_None, borderless_child_flags)) {
        const ImVec2 pos = ImGui::GetCursorScreenPos();
        const ImVec2 size = ImGui::GetContentRegionAvail();
        ImGui::InvisibleButton("##panel-resize", size);

        const bool hovered = ImGui::IsItemHovered();
        const bool active = ImGui::IsItemActive();
        if (hovered || active) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
            // Only show a thin accent line while the cursor is over the handle.
            const float line_w = std::max(1.0f, size.x * 0.5f);
            const float line_x = pos.x + (size.x - line_w) * 0.5f;
            const ImU32 col = ImGui::GetColorU32(active ? ImGuiCol_SeparatorActive : ImGuiCol_SeparatorHovered);
            ImGui::GetWindowDrawList()->AddRectFilled(
                { line_x, pos.y }, { line_x + line_w, pos.y + size.y }, col);
        }
        if (active) {
            const float dx = ImGui::GetIO().MouseDelta.x;
            const float new_width = panel_width - dx;
            // Once the panel is not wider than the handle itself, there's no content
            // left to show — and since the OS clamps the cursor at the right edge of
            // the screen, `new_width` never actually reaches 0. Treating the handle
            // thickness as the collapse threshold lets the user drag-to-hide as soon
            // as the panel has visually disappeared. `shown` flips off, the Window
            // menu's checkbox un-ticks automatically, and the nudge button takes over.
            if (new_width <= handle_w) {
                shown = false;
            } else {
                width_ratio = new_width / std::max(1.0f, work_w);
            }
        }
    }
    ImGui::EndChild();
}

// =====================================================================================
//  Sections column (first-section over second-section, or just one if the other is floating)
// =====================================================================================

void gearoenix::editor::ui::SidePanel::draw_sections_column()
{
    auto& first_section = static_cast<DockableSection&>(Entities::get());
    auto& second_section = static_cast<DockableSection&>(Components::get());
    const bool first_in_panel = first_section.is_docked();
    const bool second_in_panel = second_section.is_docked();
    const float full_h = ImGui::GetContentRegionAvail().y;

    if (first_in_panel && second_in_panel) {
        const float first_h = std::clamp(first_section_height_ratio, min_section_height_ratio, max_section_height_ratio) * full_h;

        // Zero `ItemSpacing.y` for the *entire* three-item stack (section 1 child,
        // splitter, section 2 child). This has to be active when each `EndChild`
        // fires because ImGui bakes the spacing into the cursor advance there — the
        // previous version pushed zero *after* section 1 ended, so its vertical
        // `ItemSpacing` had already been added, leaving a gap above the splitter.
        // Inside each section we re-push the theme default so the section's own
        // widgets still breathe.
        const auto& style = ImGui::GetStyle();
        const float x_spacing = style.ItemSpacing.x;
        const float default_y_spacing = style.ItemSpacing.y;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(x_spacing, 0.0f));

        // --- Section 1 ---
        if (ImGui::BeginChild("##side-panel-first-section", ImVec2(0, first_h), ImGuiChildFlags_Borders)) {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(x_spacing, default_y_spacing));
            first_section.draw_docked_body();
            ImGui::PopStyleVar();
        }
        ImGui::EndChild();

        // --- Splitter ---
        // Splitter height matches the panel's left-edge resize handle thickness so
        // both grabbers look and feel like the same widget.
        const float splitter_h = std::max(min_handle_thickness_px, handle_thickness_ratio * min_screen_dim());
        draw_sections_splitter(first_h, full_h, splitter_h);

        // --- Section 2 ---
        const bool s2_opened = ImGui::BeginChild(
            "##side-panel-second-section", ImVec2(0, 0), ImGuiChildFlags_Borders);
        if (s2_opened) {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(x_spacing, default_y_spacing));
            second_section.draw_docked_body();
            ImGui::PopStyleVar();
        }
        ImGui::EndChild();

        ImGui::PopStyleVar();
    } else if (first_in_panel) {
        first_section.draw_docked_body();
    } else if (second_in_panel) {
        second_section.draw_docked_body();
    }
}

// =====================================================================================
//  Docked panel (right-hand dock) — orchestration
// =====================================================================================

void gearoenix::editor::ui::SidePanel::draw_docked_panel()
{
    if (!wants_panel()) {
        cached_width = 0.0f;
        return;
    }

    const auto* const vp = ImGui::GetMainViewport();
    const ImVec2 work_pos = vp->WorkPos;
    const ImVec2 work_size = vp->WorkSize;

    // No clamp: the resize handle is free to grow or shrink the panel; collapsing
    // past zero is handled in `draw_panel_resize_handle` by flipping `shown` off
    // (and the nudge button on the right edge brings the panel back at `min_panel_width_px`).
    const float panel_width = std::max(0.0f, width_ratio * work_size.x);
    cached_width = panel_width;
    const float panel_x = work_pos.x + work_size.x - panel_width;

    ImGui::SetNextWindowPos({ panel_x, work_pos.y }, ImGuiCond_Always);
    ImGui::SetNextWindowSize({ panel_width, work_size.y }, ImGuiCond_Always);

    // Zero-padding / zero-spacing through the outer window + both sub-columns so the
    // resize handle hugs the viewport edge AND the section column hugs the resize
    // handle — no ambient gap. The theme defaults are restored *inside* the section
    // column, so each bordered section still gets normal interior padding for its
    // own content.
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

    if (!ImGui::Begin("##editor-side-panel", nullptr, side_panel_window_flags)) {
        ImGui::End();
        ImGui::PopStyleVar(2);
        return;
    }

    draw_panel_resize_handle(panel_width, work_size.x);
    ImGui::SameLine(0.0f, 0.0f);

    const bool sections_col_open = ImGui::BeginChild(
        "##side-panel-sections-col", ImVec2(0, 0), ImGuiChildFlags_None, borderless_child_flags);
    // The push above captures Section column frame (child WindowPadding); now
    // that we're inside it, restore the theme so nested sections get normal spacing.
    ImGui::PopStyleVar(2);
    if (sections_col_open) {
        draw_sections_column();
    }
    ImGui::EndChild();

    ImGui::End();
}

// =====================================================================================
//  Nudge button — re-shows the panel after it has been collapsed
// =====================================================================================

void gearoenix::editor::ui::SidePanel::draw_nudge_button()
{
    const auto* const vp = ImGui::GetMainViewport();
    // Sized to exactly the chevron glyph — no extra frame padding, no taller-than-icon
    // hit area. The host window also matches the icon size, so nothing else is drawn.
    const ImVec2 btn_size = ImGui::CalcTextSize(ICON_CHEVRON_LEFT);
    const ImVec2 win_pos {
        vp->WorkPos.x + vp->WorkSize.x - btn_size.x,
        vp->WorkPos.y + (vp->WorkSize.y - btn_size.y) * 0.5f,
    };

    constexpr auto nudge_window_flags = ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoScrollbar
        | ImGuiWindowFlags_NoScrollWithMouse
        | ImGuiWindowFlags_NoSavedSettings
        | ImGuiWindowFlags_NoBackground
        | ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::SetNextWindowPos(win_pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(btn_size, ImGuiCond_Always);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
    if (ImGui::Begin("##side-panel-nudge", nullptr, nudge_window_flags)) {
        if (ImGui::Button(ICON_CHEVRON_LEFT, btn_size)) {
            shown = true;
            // Width restoration is centralised in `update()` — fires for any
            // `false → true` transition (menu checkbox or nudge button).
        }
        tooltip("Show side panel");
    }
    ImGui::End();
    ImGui::PopStyleVar(2);
}

// =====================================================================================
//  Per-frame tick
// =====================================================================================

void gearoenix::editor::ui::SidePanel::update()
{
    // Whole-panel visibility gate. When `shown` is false (either toggled off in the
    // Window menu or collapsed by dragging the resize handle past the viewport
    // edge), we draw only the small re-show nudge button on the right edge and clear
    // the cached width so `Viewport` expands into the freed space.
    if (!shown) {
        cached_width = 0.0f;
        draw_nudge_button();
        return;
    }
    // Re-show transition: when the last frame had no visible panel (`cached_width == 0`)
    // and `shown` is now true, ensure the panel comes back at least at the configured
    // minimum width — independent of *which* control flipped `shown` (Window-menu
    // checkbox or the nudge button). Larger user-set widths are preserved.
    if (cached_width <= 0.0f) {
        const auto* const vp = ImGui::GetMainViewport();
        const float min_ratio = min_panel_width_px / std::max(1.0f, vp->WorkSize.x);
        if (width_ratio < min_ratio) {
            width_ratio = min_ratio;
        }
    }
    draw_docked_panel();
    Entities::get().draw_as_floating_window();
    Components::get().draw_as_floating_window();
}